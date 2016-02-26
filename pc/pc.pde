import processing.serial.*;

Serial myPort;  // Create object from Serial class
String s;     // Data received from the serial port
  
BufferedReader reader;
String line;

int t0, t1, t2, t3 = 0;


void setup(){
  printArray(Serial.list());
  String portName = "/dev/ttyUSB1"; // Serial.list()[7]; //change the 0 to a 1 or 2 etc. to match your port
  myPort = new Serial(this, portName, 115200); 
  
  reader = createReader("usb.ngc");   

}


void draw()
{
  
  
  if ( myPort.available() > 0) {  // If data is available,
   s = myPort.readStringUntil('\n');  
    
   //t1 = millis() - t0;
    
   print("[A -> P] " + s);
   if(s != "null"){
   
     if(s.equals("next\r\n")){
     
       try {
         do{line = reader.readLine();}
         while(line.length() == 0 || line.charAt(0) != 'G');
       } catch (IOException e) {
         println("Error");
         e.printStackTrace();
         line = null;
       }
       println("[P -> A] " + line);
       myPort.write(line);
      
     }
   }
  }
  
}