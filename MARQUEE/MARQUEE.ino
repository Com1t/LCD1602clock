void setup (){
   int i=1;
   while(i<=10){
     pinMode (i,OUTPUT);
     i++;
   }
 }  

void loop (){
   int i=1;
   while(i<=10){
    digitalWrite(i, HIGH);
    delay(100);
    digitalWrite(i, LOW);
    i++;   
   }
   int r=10;
   while(r>0 && r<=10) {
    digitalWrite(r, HIGH);
    delay(100);
    digitalWrite(r, LOW);
    r--;
   }
}

