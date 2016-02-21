import processing.serial.*;

Serial myPort;  // Create object from Serial class
String s;     // Data received from the serial port
  
BufferedReader reader;
String line;

void setup(){
  printArray(Serial.list());
  String portName = "/dev/ttyUSB7"; // Serial.list()[7]; //change the 0 to a 1 or 2 etc. to match your port
  myPort = new Serial(this, portName, 115200); 
  
  reader = createReader("usb.ngc");   
   
}


void draw()
{
  int i;
  String comp;
  
  if ( myPort.available() > 0) {  // If data is available,
    //println(myPort.read());
    s = myPort.readStringUntil('\n');    
    
    if(s.equals("next\r\n")) {
      try {
        do{line = reader.readLine();}
        while(line.length() == 0);
      } catch (IOException e) {
        println("Error");
        e.printStackTrace();
        line = null;
      }
      println(line);
      //if (line == null) {
        // Stop reading because of an error or file is empty
        //println("File end or error");
        //noLoop();
      //} else {
        myPort.write(line);
      //}
    }
    else{
     println(s); 
    }
  } 
  
}