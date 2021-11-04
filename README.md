# Point Cloud Viewer

Simple OpenGL program to visualize point cloud.

The input data files should be **plain text** files.

screenshot on Linux:
![](Screenshot.png)


screenshot on Windows 10:
2,300,000 points(GTX 1650)
![](Screenshot_2300K.png)

```bash
USAGE: point_cloud_viewer [OPTIONS] point_cloud 

OPTIONS:
    -n, --normals <normals>
    -c, --colors <colors>
    -h, --help <help>
    -v, --version <version>

ARGS:
    point_cloud  
```

examples usage:

```shell
point_cloud_viewer --normals bunny100k.normals bunny100k.xyz
```

dependencies:

- glad: OpenGL loader
- GLFW: window manager
- imgui: GUI library
- spdlog: debug log
- structopt: parse command line arguments
