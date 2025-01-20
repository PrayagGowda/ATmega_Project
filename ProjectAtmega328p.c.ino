

#define FOSC 16000000
#define BAUD 9600
#define MYUBBER FOSC/16/BAUD-1
void USART_Init(unsigned int ubrr)
{
   UBRR0H=(unsigned char)(ubrr>>8);
   UBRR0L=(unsigned char)ubrr;
   UCSR0B=(1<<RXEN0)|(1<<TXEN0);
   UCSR0C=(1<<USBS0)|(3<<UCSZ00);
}
void USART_Transmit(char b)
 {
   while(!(UCSR0A&(1<<UDRE0)));
   UDR0=b;
   }
 unsigned char USART_Received(void)
  {
    while(!(UCSR0A &(1<<RXC0)));
    return UDR0;
  }
void USART_Flush()
{
   unsigned char dummy;
   while(UCSR0A &(1<<RXC0))dummy=UDR0;
}
void dc_pwm(int OP)
  {
    DDRD|=(1<<PD6)|(1<<PD4);
    PORTD&=~(1<<4);
    TCCR0A|=(1<<COM0A1)|(1<<WGM01)|(1<<WGM00);
    TCCR0B|=(1<<CS00); //523 
    OCR0A=((255*OP)/100);
  }

void Manual_Mode()
{
   char Ch[]="choose the motor\n"; char b[]="D for DC motor S for Stepper motor\n",K; 
   char a[]="Manual mode is selected\n"; char g[]="No Motors";
   char z[]="Do you want to continue any motor Y/N",t;
   
   for(int i=0;a[i]!='\0';i++)
   {
       USART_Transmit(a[i]);
   }USART_Flush(); USART_Transmit('\r'); USART_Transmit('\n');//Flush to clear buffer & \r \n for new line
  l1: for(int i=0;Ch[i]!='\0';i++)
   {
       USART_Transmit(Ch[i]);
   }USART_Flush(); USART_Transmit('\r'); USART_Transmit('\n');
   for(int i=0;b[i]!='\0';i++)
   {
       USART_Transmit(b[i]);
   }USART_Flush(); USART_Transmit('\r'); USART_Transmit('\n');
    K=USART_Received();
     USART_Transmit(K);
   if(K=='S')
   stepper_motor();
   if(K=='D')
   dc_motor();
      for(int i=0;z[i]!='\0';i++)
   {
       USART_Transmit(z[i]);
   }
   USART_Flush();
    t=USART_Received();
     USART_Transmit(t);
   if(t=='Y') goto l1;
   if(t=='N')
   {
          for(int i=0;g[i]!='\0';i++)
   {
       USART_Transmit(g[i]);
   }
   USART_Flush();

   }
}

void auto_Mode()
{
  char ch[]="Auto Mode is selected"; 

  for(int i=0;ch[i]!=0;i++)
  {
    USART_Transmit(ch[i]);
   }USART_Flush(); USART_Transmit('\r'); USART_Transmit('\n');
   
   ir();

   gas_sensor();
   _delay_ms(100);
   temp_sensor();
    
} 

void gas_sensor()
{
    DDRD=(1<<7);
    int x,i;  char a[]="Threshold gas is = 4%\r\n";  char b[]="gas = "; char res[3];
    for(i=0;a[i]!='\0';i++)
    {
      USART_Transmit(a[i]);
      _delay_ms(10);
     }
     for(i=0;b[i]!='\0';i++)
    {
      USART_Transmit(b[i]);
      _delay_ms(10);
     }  
   ADMUX=(1<<6)|(1<<0);
   ADCSRA=(7<<0);  ADCSRA|=(1<<7); ADCSRA|=(1<<6);
   while((ADCSRA & (1<<4))==0);
   x=ADC;  x=((x*100)/1024);
   sprintf(res,"%d",x);
   for(i=0;res[i]!='\0';i++)
    {
       USART_Transmit(res[i]);
       _delay_ms(10);
    }   USART_Transmit(' ');  USART_Transmit('%');  USART_Transmit('\r');  USART_Transmit('\n');
   if(x>4)  
    PORTD|=(1<<7);
}

void temp_sensor()
{
    int x,i;  char a[]="Threshold Temperature is = 26 C\r\n";  char b[]="temperature = "; char res[3];
    for(i=0;a[i]!='\0';i++)
    {
      USART_Transmit(a[i]);
      _delay_ms(10);
     }
     for(i=0;b[i]!='\0';i++)
    {
      USART_Transmit(b[i]);
      _delay_ms(10);
     }  
   ADMUX=(1<<6);
   ADCSRA=(7<<0);  ADCSRA|=(1<<7); ADCSRA|=(1<<6);
   while((ADCSRA & (1<<4))==0);
   x=ADC;  x=((x*5*100)/1024);
   sprintf(res,"%d",x);
   for(i=0;res[i]!='\0';i++)
    {
       USART_Transmit(res[i]);
       _delay_ms(10);
    }   USART_Transmit(' ');  USART_Transmit('C');  USART_Transmit('\r');  USART_Transmit('\n');
   if(x>15)  
   clk_dc(90);
   else      
   clk_dc(60);
}

void ir()
{
  int i;char s[]="object was detected";
  char d[]="object was not detected";
  DDRD=(1<<5);
  if((PIND&(1<<5))==0)
  {
    for(i=0;s[i]!='\0';i++)
    {
      USART_Transmit(s[i]);
    }
    Stepper_Clock(90);
  }
  if((PIND&(1<<5))!=0)
  {
    for(i=0;d[i]!='\0';i++)
    {
      USART_Transmit(d[i]);
    }
   Stepper_Anti(60);
 }
}

void dc_motor()
 {
    char x[]="enter the duty cycle\n"; 
    char y[]="dc motor is selected";
    char n[3];
    int L,i;
    char d;
    char h[]="C for clock A for anticlock";
    //char z[]="select clockwise or anti clockwise"; 
    for(i=0;y[i]!='\0';i++)
   {
       USART_Transmit(y[i]);
   }
   USART_Flush(); USART_Transmit('\r'); USART_Transmit('\n');
   //clk_dc();
   for(i=0;x[i]!='\0';i++)
   {
       USART_Transmit(x[i]);
   }
   USART_Flush(); USART_Transmit('\r'); USART_Transmit('\n');
   for(i=0;i<2;i++)
   {
       n[i]=USART_Received();
       USART_Transmit(n[i]);
   } 
   n[i]='\0';
  
   L=atoi(n);
    //printf("%d",L)

  // dc_pwm(L);
   for(int i=0;h[i]!='\0';i++)
   {
       USART_Transmit(h[i]);
   }USART_Flush(); USART_Transmit('\r'); USART_Transmit('\n');
    d=USART_Received();
     USART_Transmit(d);
     if(d=='C')
     clk_dc(L);
     if(d=='A')
     anti_dc(L);
     //if(d=='P');
     //dc_pwm(L);
        
}

void clk_dc(int D) 
{
   DDRD|=(1<<PD6)|(1<<PD4);
   PORTD&=~(1<<6);
   TCCR0A|=(1<<COM0A1)|(1<<WGM01)|(1<<WGM00);
    TCCR0B|=(1<<CS00);
    OCR0A=((255*D)/100);
}

void anti_dc(int R)
{
  DDRD|=(1<<PD6)|(1<<PD4);
  PORTD&=~(1<<4);
 TCCR0A|=(1<<COM0A1)|(1<<WGM01)|(1<<WGM00);
    TCCR0B|=(1<<CS00);
    OCR0A=((255*R)/100);
}

void stepper_motor()
{
     char a[]="enter a angle";
     char b[]="Stepper motor is selected"; 
     char msg[]="Select Clockwise or anticlockwise";
     char g[]="C for clock A for anticlock";
     int i,A;
     char n[3];
     char d;
      for(i=0;b[i]!='\0';i++)
   {
       USART_Transmit(b[i]);
   }
   USART_Flush(); USART_Transmit('\r'); USART_Transmit('\n');

       for(i=0;a[i]!='\0';i++)
   {
       USART_Transmit(a[i]);
   }
   USART_Flush(); USART_Transmit('\r'); USART_Transmit('\n');
   for(i=0;i<2;i++)
   {
       n[i]=USART_Received();
       USART_Transmit(n[i]);
   } 
   n[i]='\0';
  
   A=atoi(n);
   // stepper_Clock(A);

 for(int i=0;g[i]!='\0';i++)
   {
       USART_Transmit(g[i]);
   }USART_Flush(); USART_Transmit('\r'); USART_Transmit('\n');
    d=USART_Received();
     USART_Transmit(d);

     if(d=='C')
        Stepper_Clock(A);
     if(d=='A')
        Stepper_Anti(A);
}
       
void Stepper_Clock(int n)
{
     DDRB=(0x0f);
     PORTB&=~(0x0f);
     int i;

    for(i=0;i<=(n/7.2);i++)
    {
      PORTB=(1<<0);
      _delay_ms(100);
      PORTB&=~(1<<0);

      PORTB=(1<<1);
      _delay_ms(100);
      PORTB&=~(1<<1);

      PORTB=(1<<2);
      _delay_ms(100);
      PORTD&=~(1<<2);

      PORTB=(1<<3);
      _delay_ms(100);
      PORTB&=~(1<<3);
      }
}

void Stepper_Anti(int n)
{
      DDRB=(0x0f);
      PORTB&=~(0x0f);
      int i;

    for(i=0;i<=(n/7.2);i++)
    {
      PORTB=~(1<<3);
      _delay_ms(100);
      PORTB&=(1<<3);

      PORTB=~(1<<2);
      _delay_ms(100);
      PORTB&=(1<<2);

      PORTB=~(1<<1);
      _delay_ms(100);
      PORTB&=(1<<1);

      PORTB=~(1<<0);
      _delay_ms(100);
      PORTB&=(1<<0);
       }
}

int main()
{
  USART_Init(MYUBBER);
  char a[]="Enter mode M for Manual mode or A for Auto mode ";char b[]="Do you want continue any mode y/n or For Exit Press q";
  char P,M;char c[]="No mode is Selected"; char d[]="Thank you...!";
  l2:for(int i=0;a[i]!='\0';i++)
   {
       USART_Transmit(a[i]);
   }
   USART_Flush();  USART_Transmit('\r'); USART_Transmit('\n');
   P=USART_Received();

  if(P=='M')
      Manual_Mode();
 if(P=='A')
    auto_Mode(); 
  for(int i=0;b[i]!='\0';i++)
   {
       USART_Transmit(b[i]);
   }
   USART_Flush();  USART_Transmit('\r'); USART_Transmit('\n');
   M=USART_Received();
    USART_Transmit(M);
    if(M=='y') goto l2;
    else
    {
       for(int i=0;c[i]!='\0';i++)
   {
       USART_Transmit(c[i]);
   }
   USART_Flush();  USART_Transmit('\r'); USART_Transmit('\n');
    }
   if(M=='q')
   {
     OCR0A=0; PORTD&=~(1<<7);
    for(int i=0;d[i]!='\0';i++)
   {
       USART_Transmit(d[i]);
   }
   USART_Flush();  USART_Transmit('\r'); USART_Transmit('\n');
   }

}