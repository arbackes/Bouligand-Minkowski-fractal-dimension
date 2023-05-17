# Bouligand-Minkowski-fractal-dimension
Program to compute Bouligand-Minkowski fractal dimension from a 3D model (OBJ)

This program can process only Wavefront .obj files as displayed below

![fig01](https://github.com/arbackes/Bouligand-Minkowski-fractal-dimension/assets/54945366/c18e94ff-66c2-40fc-a0f6-67123feb2f64)

After executing the program processOBJ.exe, click in Select OBJ files

![fig02](https://github.com/arbackes/Bouligand-Minkowski-fractal-dimension/assets/54945366/40ea83f7-79f6-435e-b1e0-69fb8115cb1a)

Select as many file you wich to process

![fig03](https://github.com/arbackes/Bouligand-Minkowski-fractal-dimension/assets/54945366/412e204f-2f09-48b5-83c3-89ff261ad1de)

Click in Process OBJ files to compute the Bouligand-Minkowski for each OBJ file. The green bar indicates when it is done

![fig04](https://github.com/arbackes/Bouligand-Minkowski-fractal-dimension/assets/54945366/587b89f0-0881-48d0-bfd9-f46902c37c4a)

For each file a text file with the resulting data is created

![fig05](https://github.com/arbackes/Bouligand-Minkowski-fractal-dimension/assets/54945366/a5e10023-b5c5-4ded-b943-c1d01a807b0e)

Resulting data: line_ID Dilation_Radius log(Dilation_Radius) log(Influence_Volume)

![fig06](https://github.com/arbackes/Bouligand-Minkowski-fractal-dimension/assets/54945366/d9a35cd1-dce4-47dd-b253-5e1dd00e5db0)

Fractal Dimension D = 3-S, where S is the slope of the line that fits the curve log(Dilation_Radius) x log(Influence_Volume)
