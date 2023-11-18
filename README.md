# Bienvenue sur la page de téléchargement des fichiers du logiciel Kinattente
### Le fichier .exe pour intaller kinattente dans sa dernière version se trouve dans "Versions Finales", choisir "Kinattente VX.X" avec la version la plus récente
Ici vous trouverez les fichiers qui m'ont servi à créer tout le logiciel, voici le listing de ce que vous pourrez trouver dans les dossiers :
### Illustrations
Ce sont les images faits sous Illustrator ainsi que leur fichiers .ai.

### Integrid
Contient un fichier qss (QStyleSheet) donc CSS que j'avais envisagé comme style possible pour le logiciel lors de mes recherches, il n'est pas optimisé.

### Kinattente
C'est le dossier principal avec le code source du logiciel, la version Release et la version Debug généré par Qt.
- Debug
- Release
- Kinattente : tous les codes sources du logiciel et tous les fichiers nécessaires à sa compilation. On retrouvera donc les images et les bibliothèques Qt et autres indispensables au focntionnement du logiciel.

### Nettoyage Settings R
C'est le Release d'un petit programme permettant de nettoyer le registre de la présence de Kinattente et de retirer l'ordre de lancer Kinattente au démarrage de l'ordinateur, je l'utilisais pendant mes essais.

### Tests
Contient les fichiers du petit programme « Nettoyage Settings » (qui s'appelle dans ce dossier "Tests"), il y a le dossier code source, le Debug et le Release.

### Updater
C'est un programme pour mettre à jour Kinattente plus tard.

### Versions Finales
Dossier contenant toutes les versions précédentes et actuelle du logiciel Kianttente :
-	Kinattente V2.0 : Release V2.0, soit le logiciel avant création du setup
-	Kinattente V3.0 : Release V3.0, soit le logiciel avant création du setup
-	Reliquat : contient une ancienne commande .bat utilisée pour forcer le lancement de l'installation des bibliothèques VC_redist (fichier .msi) lors de l'installation de Kinattente, parce que je n'arrivais pas à faire autrement. Aujourd'hui j'ai trouvé un VC_redist.exe directement inclus dans le dossier de Kinattente. Le dossier contient également les bibliothèques VC_redist associées (mysql-installer-web-community-8.0.23.0.msi).
-	Kinattente_setup 1.0.exe (V 1.0)
-	Kinattente_setup 1.0.iss (…)
-	Kinattente V2.0.exe (V 2.0)
-	Kinattente V2.0.iss (fichier création setup Inno Setup Script)
-	Kinattente V3.0.exe (V 3.0)
-	Kinattente V3.0.iss (fichier création setup Inno Setup Script)

### Creation_Tables.sql
C'est un fichier test pour créer la base de données, les accès, les tables … de MySQL. Maintenant c’est FillUpKinattente.sql le fichier officiel qui se trouve directement dans les fichiers du logiciel. Je le garde parce que je ne sais plus s'il est important.

### data.sqlite.sql
C'est un fichier sql pour créer la structure des tables de SQLite.


<!--
**Kinattente/Kinattente** is a ✨ _special_ ✨ repository because its `README.md` (this file) appears on your GitHub profile.

Here are some ideas to get you started:

- 🔭 I’m currently working on ...
- 🌱 I’m currently learning ...
- 👯 I’m looking to collaborate on ...
- 🤔 I’m looking for help with ...
- 💬 Ask me about ...
- 📫 How to reach me: ...
- 😄 Pronouns: ...
- ⚡ Fun fact: ...
-->
