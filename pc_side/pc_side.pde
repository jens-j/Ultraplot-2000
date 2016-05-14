import processing.serial.*;

Serial myPort;  // Create object from Serial class
String s;     // Data received from the serial port
  
BufferedReader reader;
String line;

int t0, t1, t2, t3 = 0;


void setup(){
  //print(Serial.list());
  String portName = "/dev/ttyUSB0"; 
  //String portName = Serial.list()[0]; //change the 0 to a 1 or 2 etc. to match your port
  myPort = new Serial(this, portName, 115200); 
  
  reader = createReader("../gcode/octopus_offset.ngc");   
  
}


void draw()
{
  
  
  if ( myPort.available() > 0) {  // If data is available,
   s = myPort.readStringUntil('\n');  
    
   //t1 = millis() - t0;
   
   if(s != null){
     print("[A -> P] " + s);
     if(s.equals("next\r\n")){
     
       // try to read a GCode command from the file
       try {
         do{line = reader.readLine();}
         while(line.length() == 0 || line.charAt(0) != 'G'); // retry if the line is not a GCode command
       } catch (IOException e) {
         println("Error");
         e.printStackTrace();
         line = null;
       }
       
       // send the string until it is echoed correctly      
       while(true){
         
         println("\n[P -> A] " + line);
         myPort.write(line);
         
         s = null;
         while(s == null){
           while( myPort.available() == 0 ){
             delay(1);
             //println("wait");
           }
           s = myPort.readStringUntil('\n'); 
         }
         print("[A -> P] " + s);
         
         if(s.indexOf(line) != -1){  
           myPort.write("good");
           println("[P -> A] good");
           break;
         } else {
           myPort.write("bad");
           println("[P -> A] bad");
         }
         delay(1000);
       }
     }
    }
  }
}