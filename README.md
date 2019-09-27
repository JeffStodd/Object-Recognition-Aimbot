# Object-Recognition-Aimbot
Utilizes YOLO Darknet, CUDA, OPENCV, and Windows library to aim for user in First Person Shooter games at the target that has been trained for the Neural Network.

Demo:
<img src="
https://cdn.discordapp.com/attachments/489642661347721216/515039976573108224/ezgif-4-c7e8efc7f2be.gif" class="img-responsive" alt=""> </div>

Discontinued due to absurd computing power required to run efficiently.
GTX 1060 can barely run at 40 fps with yolo tiny weights leading to inaccurate aiming.

Current unresolved issues:
Object detector will often lose track of nearest object for a very short duration (milliseconds) and try to aim at the next detected object.
This leads glitchy aiming between multiple targets (see demo).
