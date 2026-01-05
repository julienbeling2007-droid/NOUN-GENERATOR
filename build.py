"""
Script de compilation simplifié pour le projet C++
"""

import os
import subprocess
import sys
import shutil
from pathlib import Path

# Configuration
PROJECT_NAME = "random_word_generator"
SRC_DIR = "src"
INCLUDE_DIR = "include"
BUILD_DIR = "build"
BIN_DIR = "bin"

# Fichiers sources
SOURCES = [
    "src/main.cpp",
    "src/generator.cpp",
    "src/ui.cpp",
    "src/imgui_impl_sdl3.cpp",
    "src/imgui_impl_sdlrenderer3.cpp",
    "src/imgui/imgui.cpp",
    "src/imgui/imgui_demo.cpp",
    "src/imgui/imgui_draw.cpp",
    "src/imgui/imgui_tables.cpp",
    "src/imgui/imgui_widgets.cpp"
]

def clean():
    """Nettoie les fichiers de build"""
    if os.path.exists(BUILD_DIR):
        shutil.rmtree(BUILD_DIR)
    if os.path.exists(BIN_DIR):
        shutil.rmtree(BIN_DIR)
    print("✓ Nettoyage effectué")

def build():
    """Compile le projet"""
    # Créer les répertoires
    Path(BUILD_DIR).mkdir(exist_ok=True)
    Path(BIN_DIR).mkdir(exist_ok=True)
    
    print("🔨 Compilation en cours...\n")
    
    # Préparer les fichiers objets
    obj_files = []
    
    # Compiler chaque fichier source
    for src in SOURCES:
        if not os.path.exists(src):
            print(f"✗ Fichier manquant: {src}")
            return False
        
        # Nom du fichier objet
        obj_name = os.path.basename(src).replace(".cpp", ".o")
        obj_path = os.path.join(BUILD_DIR, obj_name)
        obj_files.append(obj_path)
        
        # Commande de compilation
        cmd = [
            "g++", "-c", src, "-o", obj_path,
            "-std=c++17", "-O2",
            f"-I{INCLUDE_DIR}",
            f"-I{os.path.join(SRC_DIR, 'imgui')}"
        ]
        
        print(f"  Compiling {os.path.basename(src)}...")
        
        try:
            subprocess.run(cmd, check=True, capture_output=True)
        except subprocess.CalledProcessError as e:
            print(f"✗ Erreur de compilation: {src}")
            print(e.stderr.decode())
            return False
    
    print("\n🔗 Linkage...")
    
    # Linker tous les fichiers objets
    exe_path = os.path.join(BIN_DIR, f"{PROJECT_NAME}.exe")
    cmd = [
        "g++", "-o", exe_path
    ]
    cmd.extend(obj_files)
    cmd.extend(["-lSDL3", "-limm32", "-lole32", "-loleaut32", "-luuid", "-lgdi32"])
    
    try:
        subprocess.run(cmd, check=True, capture_output=True)
    except subprocess.CalledProcessError as e:
        print(f"✗ Erreur de linkage")
        print(e.stderr.decode())
        return False
    
    print(f"\n✅ Build réussi!")
    print(f"📦 Exécutable: {exe_path}")
    print(f"\n▶️  Lancer: ./{exe_path}")
    return True

def main():
    if len(sys.argv) > 1 and sys.argv[1] == "clean":
        clean()
        return 0
    
    if not shutil.which("g++"):
        print("✗ g++ n'est pas installé")
        return 1
    
    success = build()
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())