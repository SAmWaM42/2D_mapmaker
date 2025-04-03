# Raylib Tile Map Editor

This project is a simple tile map editor built using the Raylib game development library. It allows you to create and edit tile-based maps, which can be used in your Raylib games.

## Features

* **Tile Placement:** Place tiles on a grid or freely in the world.
* **Tile Selection:** Cycle through different tile types and variants.
* **Tile Removal:** Remove tiles from the map.
* **Grid Display:** Toggle a grid overlay for precise tile placement.
* **Camera Control:** Pan and zoom the camera to navigate the map.
* **Map Saving and Loading:** Save and load maps to JSON files.
* **Tile Layering:** Attempt to avoid layering of tiles, and if layered, shift the newly placed tile.
* **Map Editing:** Allows for editing of existing map files.
* **Map File Removal:** Allows for the removal of map files.

## Dependencies

* [Raylib](https://www.raylib.com/)
* [nlohmann/json](https://github.com/nlohmann/json)

## Building

1.  **Install Raylib and nlohmann/json:**
    * Follow the Raylib installation instructions for your operating system.
    * Include the nlohmann/json library in your project. You can either copy the `json.hpp` file into your project directory or use a package manager.

2.  **Compile the Code:**
    * Use a C++ compiler (e.g., GCC, Clang) to compile the `mapmaker.cpp` file, along with `root_classes.cpp`.
    * Ensure that you link the Raylib library during compilation.

    Example (GCC):

    ```bash
    g++ mapmaker.cpp root_classes.cpp -o mapeditor -lraylib -lGL -lm -lpthread -lX11 -lXrandr -lXi
    ```

    * Adjust the Raylib linking flags as needed for your system.

3.  **Place Assets:**
    * Create an `assets` folder in the same directory as your executable.
    * Place your tile textures in the `assets/grass/` folder or any other folders you decide to create.
    * Ensure the paths in the code match your asset locations.

## Usage

1.  **Run the Executable:**
    * Execute the compiled `mapeditor` program.

2.  **Controls:**

    * **Left Mouse Button:** Place a tile at the mouse cursor's position.
        * **Left Shift + Left Mouse Button:** Place a tile freely, not on the grid.
    * **Right Mouse Button:** Remove a tile at the mouse cursor's position.
    * **Right Shift:** Toggle tile type/variant selection mode.
    * **Left Shift:** Toggle free placement mode.
    * **N:** Cycle through tile types (Right Shift active) or tile variants (Right Shift inactive).
    * **O:** Save the map to a JSON file.
    * **G:** Toggle the grid overlay.
    * **X:** Attempt to sort the tiles to prevent layering.
    * **R:** Remove a map file. (Prompts for filename in console).
    * **E:** Edit an existing map file. (Prompts for filename in console).
    * **Camera Movement:** Use the mouse and keyboard to pan and zoom.

## File Structure

mapeditor/
├── assets/
│   └── grass/
│       └── grass.png (Tile texture)
├── mapmaker.cpp
├── root_classes.cpp
├── json.hpp
└── mapeditor (Executable)


## Notes

* Ensure that the texture file you load is in a format that raylib supports.
* The `root_classes.cpp` file contains the `mod_cam` and `mapset` classes, which handle camera control and map data.
* The `json.hpp` file is from the nlohmann/json library and is used for JSON serialization and deserialization.
* The map files are saved in the `assets/maps/` folder.
* The grid tile size is set to 40 pixels. You can change this in the `tile_size` variable.
* The amount of grid tiles is set to 100. You can change this in the `grid_tiles` variable.

## Future Improvements

* Implement tile layering management.
* Add support for multiple tile layers.
* Improve the user interface.
* Add more advanced map editing features.
* Increase the amount of supported file types for maps.

## Author

Gichuru Samuel Wamae
