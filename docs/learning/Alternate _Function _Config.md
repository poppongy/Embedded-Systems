when using temp1 and temp2, it is advised to use  8 as the denominator to signify the total number of bits that each array, GPIOx->AFR[] 
can handle. 

#Enabling transmitter. we only configure pin 3 TE of the usart_cr1 register. we do not enable RE yet because RX is not needed right now

Note:

DR = data register
once you write to it, hardware shifts the bits out automatically