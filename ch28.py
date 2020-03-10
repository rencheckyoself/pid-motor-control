# chapter 28 in python

# sudo apt-get install python3-pip
# python3 -m pip install pyserial
# sudo apt-get install python3-matplotlib

import serial
ser = serial.Serial('/dev/ttyUSB0',230400,rtscts=1)
print('Opening port: ')
print(ser.name)

has_quit = False
# menu loop
while not has_quit:
    print('PIC32 MOTOR DRIVER INTERFACE')
    # display the menu options; this list will grow
    print('\na: Read current sensor (raw) \nb: Read current sensor (mA) \nc: Read encoder (raw) \nd: read encoder (deg) \ne: reset encoder value \nq: Quit \nr: Get mode') # '\t' is a tab
    # read the user's choice
    selection = input('\nENTER COMMAND: ')
    selection_endline = selection+'\n'

    # send the command to the PIC32
    ser.write(selection_endline.encode()); # .encode() turns the string into a char array

    # take the appropriate action
    # there is no switch() in python, using if elif instead
    if (selection == 'a'):
        n_str = ser.read_until(b'\n');
        n_int = int(n_str)
        print('ADC Count: ' + str(n_int) + '\n') # print it to the screen

    elif (selection == 'b'):
        n_str = ser.read_until(b'\n');
        n_int = int(n_str)
        print('ADC mA: ' + str(n_int) + '\n') # print it to the screen

    elif (selection == 'c'): # read the raw encoder value
        n_str = ser.read_until(b'\n');
        n_int = int(n_str)
        print('Encoder Count: ' + str(n_int) + '\n') # print it to the screen

    elif (selection == 'e'): # reset the raw encoder value
        print("Encoder has been reset to 32,768")

    elif (selection == 'd'): # read the raw encoder value
        n_str = ser.read_until(b'\n');
        n_int = int(n_str)
        print('Encoder Degrees: ' + str(n_int) + '\n') # print it to the screen

    elif (selection == 'q'):
        print('Exiting client')
        has_quit = True; # exit client
        ser.close() # be sure to close the port

    elif (selection == 'r'):
        n_str = ser.read_until(b'\n');
        n_int = int(n_str)

        if n_int == 0:
            n_str = "IDLE"
        elif n_int == 1:
            n_str = "PWM"
        elif n_int == 2:
            n_str = "ITEST"
        elif n_int == 3:
            n_str = "HOLD"
        elif n_int == 4:
            n_str = "TRACK"

        print('Current Mode: ' + n_str + '\n')

    # elif (selection == 'x'):
    #     n_str = input('Enter number: ') # get the number to send
    #     n_int = int(n_str) # turn it into an int
    #     print('number = ' + str(n_int)) # print it to the screen to double check
    #
    #     n_str2 = input('Enter number: ') # get the number to send
    #     n_int2 = int(n_str2) # turn it into an int
    #     print('number = ' + str(n_int2)) # print it to the screen to double check
    #
    #     n_both = n_str + ' ' + n_str2
    #
    #     ser.write((n_both + '\n').encode())
    #
    #     n_str = ser.read_until(b'\n');  # get the incremented number back
    #     n_int = int(n_str) # turn it into an int
    #     print('Got back: ' + str(n_int) + '\n') # print it to the screen
    else:
        print('Invalid Selection ' + selection_endline)
