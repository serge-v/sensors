Temperature Measurement
=======================

17.3   Operation

The  ADC  converts  an analog input voltage to a 10-bit digital 
value  through  successive  approximation.  The  minimum  value 
represents  GND  and  the  maximum value represents the voltage 
on  VCC,  the  voltage  on  the  AREF pin or an internal 1.1V / 
2.56V voltage reference.

The  voltage  reference  for the ADC may be selected by writing 
to  the  REFS[2:0]  bits  in  ADMUX.  The  VCC supply, the AREF 
pin  or  an  internal  1.1V  /  2.56V  voltage reference may be 
selected  as the ADC voltage reference. Optionally the internal 
2.56V voltage reference may be decoupled by an external capacitor 
at the AREF pin to improve noise immunity.

The  analog  input  channel  and differential gain are selected 
by  writing  to  the  MUX[3:0]  bits  in ADMUX. Any of the four 
ADC  input pins ADC[3:0] can be selected as single ended inputs 
to  the  ADC.  ADC2  or  ADC0 can be selected as positive input 
and  ADC0,  ADC1,  ADC2  or  ADC3  can  be selected as negative 
input to the differential gain amplifier.

If  differential  channels  are selected, the differential gain 
stage  amplifies  the  voltage  difference between the selected 
input  pair  by  the selected gain factor, 1x or 20x, according 
to the setting of the MUX[3:0] bits in ADMUX.

This  amplified  value  then  becomes  the  analog input to the 
ADC.  If  single  ended  channels  are used, the gain amplifier 
is bypassed altogether.

If  ADC0  or ADC2 is selected as both the positive and negative 
input to the differential gain amplifier (ADC0-ADC0 or ADC2-ADC2), 
the remaining offset in the gain stage and conversion circuitry 
can  be  measured  directly  as  the  result of the conversion. 
This  figure  can  be  subtracted  from  subsequent conversions 
with  the  same  gain  setting  to reduce offset error to below 
1 LSW.

The  on-chip  temperature  sensor  is  selected  by writing the 
code  "1111"  to  the  MUX[3:0] bits in ADMUX register when the 
ADC4 channel is used as an ADC input.

The  ADC  is  enabled  by  setting  the ADC Enable bit, ADEN in 
ADCSRA.  Voltage  reference  and  input channel selections will 
not  go into effect until ADEN is set. The ADC does not consume 
power  when  ADEN  is  cleared,  so it is recommended to switch 
off the ADC before entering power saving sleep modes.

The  ADC  generates  a  10-bit result which is presented in the 
ADC  Data  Registers,  ADCH  and  ADCL.  By default, the result 
is  presented  right  adjusted, but can optionally be presented 
left adjusted by setting the ADLAR bit in ADMUX.

If the result is left adjusted and no more than 8-bit precision 
is  required,  it  is  sufficient to read ADCH. Otherwise, ADCL 
must  be  read  first,  then  ADCH,  to ensure that the content 
of  the  data  registers  belongs  to the same conversion. Once 
ADCL  is  read,  ADC  access to data registers is blocked. This 
means  that  if  ADCL has been read, and a conversion completes 
before  ADCH  is  read,  neither  register  is  updated and the 
result from the conversion is lost.

When  ADCH  is  read, ADC access to the ADCH and ADCL Registers 
is re-enabled.

The  ADC  has  its  own  interrupt  which can be triggered when 
a  conversion  completes. When ADC access to the data registers 
is  prohibited  between reading of ADCH and ADCL, the interrupt 
will trigger even if the result is lost.

17.4   Starting a Conversion

A  single  conversion  is  started  by writing a logical one to 
the  ADC  Start  Conversion  bit,  ADSC. This bit stays high as 
long  as  the  conversion  is  in  progress and will be cleared 
by  hardware  when  the conversion is completed. If a different 
data  channel  is  selected  while a conversion is in progress, 
the  ADC  will  finish the current conversion before performing 
the channel change.

Alternatively,  a  conversion  can  be  triggered automatically 
by  various  sources.  Auto  Triggering  is  enabled by setting 
the  ADC  Auto Trigger Enable bit, ADATE in ADCSRA. The trigger 
source  is  selected  by  setting  the ADC Trigger Select bits, 
ADTS  in  ADCSRB  (see  description of the ADTS bits for a list 
of  the  trigger  sources).  When a positive edge occurs on the 
selected  trigger  signal,  the  ADC  prescaler  is reset and a 
conversion  is  started.  This  provides  a  method of starting 
conversions  at  fixed  intervals.  If the trigger signal still 
is  set  when  the  conversion completes, a new conversion will 
not  be started. If another positive edge occurs on the trigger 
signal during conversion, the edge
will  be  ignored.  Note  that  an  Interrupt  Flag will be set 
even  if  the  specific  interrupt  is  disabled  or the Global 
Interrupt  Enable  bit  in  SREG  is  cleared. A conversion can 
thus  be  triggered  without  causing  an  interrupt.  However, 
the  Interrupt  Flag  must  be  cleared  in  order to trigger a 
new conversion at the next interrupt event.

Using  the  ADC  Interrupt  Flag  as a trigger source makes the 
ADC  start  a  new conversion as soon as the ongoing conversion 
has  finished.  The  ADC  then  operates  in Free Running mode, 
constantly  sampling  and  updating  the ADC Data Register. The 
first  conversion  must  be  started  by  writing a logical one 
to  the  ADSC  bit in ADCSRA. In this mode the ADC will perform 
successive conversions independently of whether the ADC Interrupt 
Flag, ADIF is cleared or not.

If  Auto  Triggering  is  enabled,  single  conversions  can be 
started  by  writing  ADSC  in  ADCSRA to one. ADSC can also be 
used  to  determine  if  a  conversion is in progress. The ADSC 
bit  will  be  read  as  one during a conversion, independently 
of how the conversion was started.

ATtiny25/45/85 [DATASHEET]
2586Q-AVR-08/2013

Resistor color codes
====================

Brown Black Orange Gold   1 0 1k  5% 10k
Brown Black Red Gold      1 0 100 5% 1k
Brown Black Yellow Gold   1 0 10k 5% 100k
Brown Green Orange Gold   1 6 1k  5% 15k
Orange Orange Brown Gold  3 3 10  5% 330
Orange Orange Red Gold    3 3 100 5% 3.3k
Yellow Violet Yellow Gold 4 7 10k 5% 470k
Yellow Violet Red Gold    4 7 100 5% 4.7k

