BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS "Kines" (
	"id"	INTEGER NOT NULL,
	"name"	TEXT NOT NULL,
	"firstname"	TEXT NOT NULL,
	"color"	TEXT NOT NULL DEFAULT '#000000',
	"active"	INTEGER NOT NULL DEFAULT 1,
	"active_string"	TEXT NOT NULL DEFAULT 'Oui',
	PRIMARY KEY("id" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "Cities" (
	"id"	INTEGER NOT NULL,
	"name"	TEXT NOT NULL,
	PRIMARY KEY("id" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "Active_Patients" (
	"id"	INTEGER NOT NULL,
	"location"	INTEGER NOT NULL DEFAULT 0,
	"gender"	INTEGER NOT NULL DEFAULT 0,
	"age"	REAL NOT NULL DEFAULT 0,
	"name"	TEXT,
	"firstname"	TEXT,
	"phone1"	TEXT,
	"phone2"	TEXT,
	"email"	TEXT,
	"doctor"	TEXT,
	"kine_id"	INTEGER NOT NULL,
	"availability"	TEXT,
	"address"	TEXT,
	"city_id"	INTEGER NOT NULL,
	"pathology"	TEXT,
	"no_emergency"	INTEGER NOT NULL DEFAULT 1,
	"call_date"	TEXT NOT NULL DEFAULT '1001-01-01 00:00:00',
	CONSTRAINT "fk_patient_actifs_kine_id" FOREIGN KEY("kine_id") REFERENCES "Kines"("id"),
	CONSTRAINT "fk_patient_actifs_ville_id" PRIMARY KEY("id" AUTOINCREMENT)
);
CREATE TABLE IF NOT EXISTS "Archived_Patients" (
	"id"	INTEGER NOT NULL,
	"location"	TEXT NOT NULL DEFAULT 'Cabinet',
	"gender"	INTEGER NOT NULL DEFAULT 0,
	"age"	REAL NOT NULL DEFAULT 0,
	"firstname"	TEXT,
	"name"	TEXT,
	"phone1"	TEXT,
	"phone2"	TEXT,
	"email"	TEXT,
	"doctor"	TEXT,
	"kine_id"	INTEGER NOT NULL,
	"availability"	TEXT,
	"address"	TEXT,
	"city_id"	INTEGER NOT NULL,
	"pathology"	TEXT,
	"emergency"	TEXT NOT NULL DEFAULT 'Non',
	"call_date"	INTEGER NOT NULL DEFAULT '1001-01-01 00:00:00',
	"archiving_date"	TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
	CONSTRAINT "fk_patient_archives_kine_id" FOREIGN KEY("kine_id") REFERENCES "Kines"("id"),
	CONSTRAINT "fk_patient_archives_ville_id" FOREIGN KEY("city_id") REFERENCES "Cities"("id"),
	PRIMARY KEY("id" AUTOINCREMENT)
);
INSERT INTO "Kines" VALUES (1,'AUCUN','','#808080',1,'Oui');
INSERT INTO "Cities" VALUES (1,' ');
CREATE INDEX IF NOT EXISTS "ind_actif_nom" ON "Kines" (
	"active"	ASC,
	"name"	ASC
);
CREATE INDEX IF NOT EXISTS "ind_nom_kine" ON "Kines" (
	"name"	ASC
);
CREATE INDEX IF NOT EXISTS "ind_nom_ville" ON "Cities" (
	"name"	ASC
);
CREATE INDEX IF NOT EXISTS "ind_lieu_urgent_date" ON "Active_Patients" (
	"location"	ASC,
	"no_emergency"	ASC,
	"call_date"	ASC
);
COMMIT;
