# Számítógép grafika

# Féléves feladat projektterve: Interaktív Rubik-kocka szimuláció

## 1. Koncepció
A projekt egy atmoszferikus, 3 dimenziós interaktív jelenetet valósít OpenGL alapokon. A középpontban egy részletesen kidolgozott, működő Rubik-kocka áll, amely egy sötét és ködös szobában egy asztalon helyezkedik el. A kockát és környzeteét egy asztali lámpa világítja meg. Az egyszerűség kedvéért egy standard 3x3-as Rubik-kockát fogok alapul venni. 

## 2. Technikai Specifikáció

### Szoftverstruktúra
- **Moduláris felépítés:** `main.c`, `camera.c`, `cube.c`, `lighting.c`, `scene.c`, `texture.c`, `utils.c`.
- **Adatvezérelt megjelenítés:** A Rubik-kocka 27 darab egyedi pozíciójú és állapotú al-egységből áll, melyek adatait tömbökben fogom tárolni és ciklusokkal dolgozam majd fel őket.

### Megjelenítés és interakció
- **Kamerakezelés:** FPS-stílusú szabad kamera (WASD + egér), `delta_time` alapú sima mozgással.
- **Modellek és Textúrák:** Külső `.obj` fájlok betöltése (asztal, lámpa, szoba). A textúrák PNG/JPEG formátumúak.
- **Fények:** Pozicionált spotlight. A fény intenzitása és spekuláris komponense a `+`/`-` gombokkal dinamikusan állítható.
- **Animációk:** A kocka forgatása nem diszkrét lépésekben, hanem folyamatos, időalapú interpolációval történik.

## 3. Többletfunkciók
1. **Mouse Picking (Egérrel való kijelölés):** A kocka elemei kattintással kiválaszthatóak és az egér húzásával forgathatóak (színkódos picking vagy raycasting módszerrel).
2. **Dinamikus Köd:** Exponenciális köd effektus implementálása a mélységérzet és az atmoszféra javítása érdekében.
3. **Részecskerendszer:** A forgatások végén vagy a kocka kirakásakor apró konfetti darabok vagy szikrák jelennek meg a kocka élei mentén.

## 4. Tervezett mappaszerkezet
- `src/`: Forrásfájlok (.c)
- `include/`: Header állományok (.h)
- `assets/`: 
  - `models/`: .obj és .mtl fájlok
  - `textures/`: .png képek
- `Makefile`: A projekt fordításához
