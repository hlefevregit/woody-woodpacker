# 🪵 Woody-Woodpacker

**Woody-Woodpacker** est un packer d'exécutables ELF 64-bit qui chiffre la section `.text` d'un binaire et injecte un stub de déchiffrement. Lorsque le binaire modifié est exécuté, il affiche "....WOODY...." avant de déchiffrer et d'exécuter le code original.

---

## 📋 Table des matières

- [Principe de fonctionnement](#-principe-de-fonctionnement)
- [Architecture du projet](#-architecture-du-projet)
- [Flux d'exécution](#-flux-dexécution)
- [Détails techniques](#-détails-techniques)
- [Compilation et utilisation](#-compilation-et-utilisation)
- [Structure du code](#-structure-du-code)
- [Limitations et notes](#-limitations-et-notes)

---

## 🎯 Principe de fonctionnement

Le projet implémente un **packer binaire** classique en 3 étapes :

1. **Chiffrement** : XOR de la section `.text` avec une clé
2. **Injection** : Ajout d'un stub de déchiffrement à la fin du binaire
3. **Redirection** : Modification du point d'entrée pour pointer vers le stub

Au runtime, le stub déchiffre la section `.text` en mémoire puis saute vers le point d'entrée original.

---

## 📁 Architecture du projet

```
woody/
├── includes/
│   ├── woody.h          # Structures et prototypes
│   └── colors.h         # Macros pour les couleurs terminal
├── src/
│   ├── main.c           # Point d'entrée et validation ELF
│   ├── elf_process.c    # Logique principale du packer
│   └── encrypt.asm      # Routine de chiffrement XOR en ASM
├── Makefile             # Compilation du projet
└── README.md            # Ce fichier
```

---

## 🔄 Flux d'exécution

### Phase 1 : Packing (woody-woodpacker)

```
Binaire original (input)
         ↓
1. Validation ELF 64-bit
         ↓
2. Localisation de .text
         ↓
3. Chiffrement XOR de .text
         ↓
4. Modification des flags PT_LOAD (ajout de W)
         ↓
5. Injection du stub à la fin
         ↓
6. Extension du dernier segment PT_LOAD
         ↓
7. Patch du stub avec métadonnées
         ↓
8. Modification du point d'entrée (e_entry)
         ↓
Binaire packé (woody)
```

### Phase 2 : Exécution (woody)

```
Lancement de ./woody
         ↓
1. Exécution du stub injecté
         ↓
2. Affichage de "....WOODY...."
         ↓
3. Déchiffrement de .text en mémoire
         ↓
4. Saut vers le point d'entrée original
         ↓
Le programme original s'exécute normalement
```

---

## 🔧 Détails techniques

### Structures principales (`woody.h`)

```c
typedef struct s_key {
    uint8_t  key[64];      // Clé de chiffrement (max 64 octets)
    size_t   key_size;     // Taille effective de la clé
} t_key;

typedef struct s_woody {
    Elf64_Ehdr  *mapped_file;      // Header ELF mappé
    Elf64_Off   file_size;          // Taille du fichier
    Elf64_Shdr  *text_section;      // Pointeur vers .text
    Elf64_Phdr  *last_load_segment; // Dernier segment PT_LOAD
    uint8_t     *buffer;            // Buffer de travail
    size_t      buffer_size;        // Taille du buffer
    t_key       key;                // Clé de chiffrement
} t_woody;
```

### Algorithme de chiffrement

**Implémentation** : XOR simple avec rotation de clé (voir [encrypt.asm](src/encrypt.asm))

```
Pour chaque octet i de .text :
    text[i] ^= key[i % key_size]
```

La clé par défaut est : `{0x42, 0x37, 0x13, 0xA5, 0x5C, 0x9E, 0xFF, 0x00}` (8 octets)

### Le payload/stub

Le stub est pré-compilé et stocké dans `payload[]` ([elf_process.c](src/elf_process.c#L15))

**Tâches du stub :**
1. Sauvegarde des registres (push)
2. Affichage de "....WOODY...." via syscall write
3. Calcul de l'adresse de `.text`
4. Déchiffrement XOR en boucle
5. Restauration des registres (pop)
6. Saut vers l'entry point original

**Offsets de patch dans le stub :**
```c
#define STUB_ENTRY_DELTA_OFFSET  0x9d  // Offset vers entry original
#define STUB_TEXT_DELTA_OFFSET   0xa5  // Offset vers .text
#define STUB_TEXT_SIZE_OFFSET    0xad  // Taille de .text
#define STUB_KEY_SIZE_OFFSET     0xb5  // Taille de la clé
#define STUB_KEY_DATA_OFFSET     0xbd  // Données de la clé
```

### Modifications du binaire

1. **Flags des segments** :
   - `.text` PT_LOAD : ajout de `PF_W` (writable)
   - Dernier PT_LOAD : ajout de `PF_X` (executable)

2. **Section headers** : Aucune modification (on travaille uniquement sur les program headers)

3. **Entry point** : `e_entry` → adresse virtuelle du stub

---

## 🛠️ Compilation et utilisation

### Prérequis

- **GCC** (avec support 64-bit)
- **NASM** (Netwide Assembler)
- Système Linux (testé sur x86_64)

### Compilation

```bash
make
```

Cela génère l'exécutable `woody-woodpacker`.

### Utilisation

```bash
./woody-woodpacker <binaire_input>
```

**Exemple :**
```bash
# Créer un binaire de test
echo 'int main() { return 42; }' > test.c
gcc test.c -o test

# Packer le binaire
./woody-woodpacker test

# Exécuter le binaire packé
./woody
# Affiche: ....WOODY....
# Puis exécute le code original
```

### Nettoyage

```bash
make clean   # Supprime les fichiers objets
make fclean  # Supprime tout (binaires inclus)
make re      # Recompile tout
```

---

## 📖 Structure du code

### [src/main.c](src/main.c)

**Rôle** : Point d'entrée et validation

- `check_elf()` : Vérifie le magic number ELF, la classe 64-bit et le type (ET_EXEC/ET_DYN)
- `main()` : Gère les arguments, ouvre et mappe le fichier avec `mmap()`, appelle `process_elf()`

### [src/elf_process.c](src/elf_process.c)

**Rôle** : Cœur du packer

**Étapes détaillées** :

1. **Validation** : Appel à `check_elf()`
2. **Analyse des headers** : Parcours des Program/Section headers
3. **Localisation de `.text`** : Recherche par nom dans la string table
4. **Modification des flags** : Rendre `.text` writable
5. **Chiffrement** : Appel à `encrypt()` sur `.text`
6. **Injection du payload** : `realloc()` pour agrandir le buffer et `memcpy()` du stub
7. **Calcul des adresses** : `stub_vaddr`, `entry_delta`, `text_delta`
8. **Patch du stub** : Écriture des métadonnées aux offsets définis
9. **Extension du segment** : Mise à jour de `p_filesz` et `p_memsz`
10. **Modification du entry point** : `eh->e_entry = stub_vaddr`
11. **Écriture** : Création du fichier `woody` avec les bonnes permissions (0755)

### [src/encrypt.asm](src/encrypt.asm)

**Rôle** : Chiffrement/déchiffrement XOR

**Signature** : `void encrypt(uint8_t *data, size_t len, t_key *key)`

**Registres** :
- `rdi` : pointeur vers les données
- `rsi` : longueur
- `rdx` : pointeur vers `t_key`
- `rcx` : `key_size` (chargé depuis `[rdx + 0x40]`)
- `r8` : index `i` dans les données
- `r9` : index `j` dans la clé
- `r11b` : octet de clé temporaire

**Algorithme** :
```assembly
for i = 0 to len-1:
    data[i] ^= key[j]
    j = (j + 1) % key_size
```

### [includes/woody.h](includes/woody.h)

**Contenu** :
- Includes standard + `<elf.h>`
- Définitions des offsets dans le stub
- Structures `t_key` et `t_woody`
- Prototypes des fonctions

### [includes/colors.h](includes/colors.h)

Macros ANSI pour la colorisation de la sortie terminal (debug/info).

---

## ⚠️ Limitations et notes

### Limitations connues

1. **64-bit uniquement** : Pas de support pour les ELF 32-bit
2. **Linux x86_64** : Le stub utilise la convention d'appel System V AMD64
3. **Clé hardcodée** : La clé de chiffrement est fixe dans le code
4. **Pas de compression** : Uniquement du chiffrement, pas de réduction de taille
5. **Section `.text` seulement** : Les autres sections exécutables ne sont pas chiffrées

### Sécurité

⚠️ **Ce projet est à but éducatif uniquement.**

- Le chiffrement XOR est facilement cassable
- Le stub est détectable par les antivirus
- Les métadonnées (clé, offsets) sont en clair dans le binaire

### Debugging

Le code contient de nombreux `printf()` de debug (certains commentés) :
- Vérification des program/section headers
- Affichage des modifications de segments
- Traçage des adresses et offsets

Pour activer plus de debug, décommenter les sections commentées dans [elf_process.c](src/elf_process.c#L63-L87).

---

## 📚 Ressources

- [ELF Format Specification](https://refspecs.linuxfoundation.org/elf/elf.pdf)
- [Linux System Call Table](https://chromium.googlesource.com/chromiumos/docs/+/master/constants/syscalls.md)
- [x86-64 Instruction Reference](https://www.felixcloutier.com/x86/)
- [NASM Documentation](https://www.nasm.us/doc/)

---

## 👥 Auteur

Hugo - Projet 42

---

## 📄 Licence

Ce projet est fourni à des fins éducatives. Ne pas utiliser à des fins malveillantes.
