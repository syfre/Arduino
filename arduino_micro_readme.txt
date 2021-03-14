Arduino Pro micro

Reset twice the micro to enter bootloader
Entering bootloaded create a temp port COMx where x is Arduino micro COM +1
(so if Arduino micro is COM8, bootloader port will be COM9)
Check in windows configuration panel
The micro stay 8 seconds in bootloader mode until entering user program


Use upload.bat in C:\Program Files (x86)\Arduino\hardware\tools\avr\bin to upload :

upload 9 sketch_name
where 9 is the bootloader port
and sketch name is the project name

In Arduino IDE use "Sketch / Export compiled binary" to create the hex file
The hex file will be in the sketch folder
The hex file to upload is the one WITHOUT the bootloader

(Note : The iDE is not reliable for uploading because of the 8 seconds time frame)


