### Scene is main product of import function. Most of the assets can be accessed from hierarchy of the scene.


## Functions:


### SpawnAllMeshes (Experimental)
- Inputs:  
```
FTransform Transform 
TSubclassOf<AActor>ClassToSpawn
```
- Result: 
```
Will spawn all meshes using unreal runtime mesh.
```

### GetAllMeshes

- Result:
```
 Get All meshes stored in this scene.

Note that each material section is considered a  separate mesh .
```

### GetRootNode

- Result : 
```
The root node of the hierarchy.  
There will always be at least the root node if the import
was successful (and no special flags have been set).  
Presence of further nodes depends on the format and content
of the imported file.
```

### GetMeshAtIndex
- Inputs:  
```
int Index
```
- Result:
```
Will return mesh from index.
```

### GetAllCameras
```
- Result: Get All Cameras stored in this scene.
```