# This is an implementation from scratch of SIFT corner detection in c++ and python

## Link for Video Demo
[Demo Video](https://youtu.be/IUfXZXezEzM)

## How to Run
- For VS code users:
1. Open VS Code.
2. Download extensions named (CMake, CMake Tools).
3. Open the Project Directory on VS Code.
4. Press CTRL + SHIFT + P.
5. Choose "CMake:  Configre" -> If it asks you to choose a kit , Choose GCC 9.3.0 -> If it is not in the options then download GCC on your device.
6. Press CTRL + SHIFT + P.
7. Choose "CMake:  Build".
6. Press CTRL + SHIFT + P.
7. Choose "CMake:  Run Without Debugging".
8. Enter the full "absolute" path of the image you want to detect the edges on.
9. Go check out the results in the outputs folder.

- For UNIX terminal users
1. Create "build" directory if it does not exist
2. Run a terminal in the build folder
3. Run the command ```cmake ..```
4. Run the command ```make```
5. Run the project ```./CVProject```
6. Enter the full "absolute" path of the image you want to detect the edges on.
7. Go check out the results in the outputs folder.
