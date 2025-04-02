# Assimp Mesh Importer for Unreal Engine
![](https://img.shields.io/badge/Version-5.4.3-brightgreen)
![](https://img.shields.io/badge/Status-Under%20Development-red)
![](https://img.shields.io/badge/UE-5.0+-orange)

A powerful mesh importing solution for Unreal Engine that uses the Open Asset Import Library (Assimp) to support over 57 different file formats.

|             |  |
:-------------------------:|:-------------------------:
![](.github/Image1m.png)  |  ![](.github/Image2m.png)

## Features

### Core Functionality
- **Comprehensive Format Support**: Import from 57+ different 3D file formats 
- **Multiple Mesh Types**: Support for Static Mesh, Procedural Mesh, and Dynamic Mesh (UE 5.0+)
- **Asynchronous Loading**: Non-blocking imports to maintain performance
- **Proper Memory Management**: Full integration with Unreal Engine's garbage collection system

### Advanced Features
- **Mesh Instancing**: Optimized rendering for Static Meshes
- **Embedded Texture Import**: Automatically extract and import textures
- **Auto-generation**: Create missing normals, tangents, and other mesh data
- **Multiple Implementation Options**: Various approaches for maximum flexibility

## Getting Started

### Installation
For detailed usage examples and implementation guides, visit our [documentation](https://github.com/902D9/AssimpForUnreal/wiki).

## Mesh Type Comparison

| Mesh Type | Performance | Memory Usage | Dynamic Modification | Supported UE Version |
|-----------|-------------|--------------|----------------------|----------------------|
| Static Mesh | High | Medium | Limited | All |
| Procedural Mesh | Medium | High | Yes | All |
| Dynamic Mesh | Very High | Low | Yes | UE 5.0+ |

## Experimental Features

### Dynamic Mesh Support
Available in UE 5.0+, Dynamic Meshes offer significant performance improvements over both Static and Procedural Meshes.
- [Dynamic Mesh Documentation](https://github.com/902D9/AssimpForUnreal/wiki/Dynamic-Mesh)

### Asynchronous Importing
Use `AssimpImporter` for non-blocking, asynchronous imports:
- [Async Import Documentation](https://github.com/902D9/AssimpForUnreal/wiki/Assimp-Importer-(Async))



## Contributing

Contributions are welcome!

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- [Assimp](https://github.com/assimp/assimp) - Open Asset Import Library
- All the contributors who have helped make this plugin better
