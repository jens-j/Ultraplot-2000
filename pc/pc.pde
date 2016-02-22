import processing.serial.*;

Serial myPort;  // Create object from Serial class
String s;     // Data received from the serial port
  
BufferedReader reader;
String line;

void setup(){
  printArray(Serial.list());
  String portName = "/dev/ttyUSB14"; // Serial.list()[7]; //change the 0 to a 1 or 2 etc. to match your port
  myPort = new Serial(this, portName, 115200); 
  
  reader = createReader("usb.ngc");   
  
  try {
    do{line = reader.readLine();}
    while(line.length() == 0 || line.charAt(0) != 'G' || line.charAt(1) != '0');
  } catch (IOException e) {
    println("Error");
    e.printStackTrace();
    line = null;
  }
  println(line);
  myPort.write(line);
}


void draw()
{

  
  if ( myPort.available() > 0) {  // If data is available,
    //println(myPort.read());
    s = myPort.readStringUntil('\n');    
    println(s);
   
    if(s.equals("next\r\n")){
   
      try {
        do{line = reader.readLine();}
        while(line.length() == 0 || line.charAt(0) != 'G');
      } catch (IOException e) {
        println("Error");
        e.printStackTrace();
        line = null;
      }
      println(line);
      myPort.write(line);
    
    }
    else{
     println(s); 
    }  
  }
}