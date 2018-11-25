# GESTURE-CONTROLLED-DRONE
the drone is controlled by an accelerometer on your hand

tx/rx = nrf24L01

flight controller = multiwii built with an arduino, mpu6050

pls check the conductivity of the jumper wires using a voltmeter before connecting, cz it was one of the main reason for the frone to not work.

TX, built with a arduino pro mini, mpu6050 and a nrf24l01, comes into action when prompted by the main tx(2.4ghz 6ch flysky ct6b)

RX, built wwith an arduino mega and a nrf24l01, sends received signals to the multiwii fc

the receiver code has serial and serial1 (P.S: Its not an error, one is for serial debugging and the other is for nrf)
