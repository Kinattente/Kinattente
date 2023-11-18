CREATE DATABASE Kinattente CHARACTER SET 'utf8';

USE Kinattente;

DROP TABLE Patients_Actifs, Patients_Archives, Villes, Kines;

CREATE TABLE Kines (
    id SMALLINT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
    nom_kine VARCHAR(40) NOT NULL,
    prenom_kine VARCHAR(40) NOT NULL,
    couleur VARCHAR(7) NOT NULL DEFAULT '#000000',
    actif TINYINT NOT NULL DEFAULT 1,
    actif_string VARCHAR(3) NOT NULL DEFAULT "Oui",
    INDEX ind_actif_nom (actif, nom_kine),
    INDEX ind_nom_kine (nom_kine)
    )
    ENGINE=INNODB;

INSERT INTO Kines (id, nom_kine, prenom_kine, couleur, actif) VALUES (NULL, 'AUCUN', '', '#808080', 1);

CREATE TABLE Villes (
    id SMALLINT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
    nom_ville VARCHAR(40) NOT NULL,
    INDEX ind_nom_ville (nom_ville)
    )
    ENGINE=INNODB;

INSERT INTO Villes (id, nom_ville) VALUES (NULL, ' ');

CREATE TABLE Patients_Actifs (
	id SMALLINT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
	lieu TINYINT NOT NULL DEFAULT 0,
    sexe_patient TINYINT NOT NULL DEFAULT 0,
	nom_patient VARCHAR(20),
	prenom_patient VARCHAR(20),
	telephone1 VARCHAR(14),
	telephone2 VARCHAR(14),
	medecin VARCHAR(20),
	kine_id SMALLINT UNSIGNED NOT NULL,
	disponibilite VARCHAR(80),
	rue VARCHAR(30),
	ville_id SMALLINT UNSIGNED NOT NULL,
	pathologie VARCHAR(255),
	non_urgent TINYINT NOT NULL DEFAULT 1,
	date_appel DATETIME NOT NULL DEFAULT '1001-01-01 00:00:00',
	CONSTRAINT fk_patient_actifs_kine_id
	   FOREIGN KEY (kine_id)
	   REFERENCES Kines(id),
	CONSTRAINT fk_patient_actifs_ville_id
	   FOREIGN KEY (ville_id)
	   REFERENCES Villes(id),
	INDEX ind_lieu_urgent_date (lieu, non_urgent, date_appel)
    )
    ENGINE=INNODB;

CREATE TABLE Patients_Archives (
	id SMALLINT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
	lieu VARCHAR(8) NOT NULL DEFAULT "Cabinet",
	sexe_patient TINYINT NOT NULL DEFAULT 0,
	nom_patient VARCHAR(20),
	prenom_patient VARCHAR(20),
	telephone1 VARCHAR(14),
	telephone2 VARCHAR(14),
	medecin VARCHAR(20),
	kine_id SMALLINT UNSIGNED NOT NULL,
	disponibilite VARCHAR(36),
	rue VARCHAR(30),
	ville_id SMALLINT UNSIGNED NOT NULL,
	pathologie VARCHAR(255),
	urgence VARCHAR(3) NOT NULL DEFAULT "Non",
	date_appel DATETIME NOT NULL DEFAULT '1001-01-01 00:00:00',
	date_archivage DATETIME NOT NULL DEFAULT NOW(),
	CONSTRAINT fk_patient_archives_kine_id
	   FOREIGN KEY (kine_id)
	   REFERENCES Kines(id),
	CONSTRAINT fk_patient_archives_ville_id
	   FOREIGN KEY (ville_id)
	   REFERENCES Villes(id)
    )
    ENGINE=INNODB;
    
#    RENAME TABLE Kines TO Kines3,
#				 Kines2 TO Kines;
