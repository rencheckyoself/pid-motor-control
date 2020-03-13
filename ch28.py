# chapter 28 in python

# sudo apt-get install python3-pip
# python3 -m pip install pyserial
# sudo apt-get install python3-matplotlib
import matplotlib.pyplot as plt
import serial
ser = serial.Serial('/dev/ttyUSB0',230400,rtscts=1)
print('Opening port: ')
print(ser.name)

has_quit = False
# menu loop
while not has_quit:
    print('PIC32 MOTOR DRIVER INTERFACE')
    # display the menu options; this list will grow
    print('\na: Read current sensor (raw) \t\tb: Read current sensor (mA)\
           \nc: Read encoder (raw)        \t\td: read encoder (deg) \
           \ne: reset encoder value       \t\tf: Set PWM \
           \ng: Set currnet gains         \t\th: Get current gains \
           \ni: Set position gains        \t\tj: Get position gains \
           \nk: Test current control      \t\tl: Go to anlge (deg) \
           \nm: Load step trajectory      \t\tn: Load qubic trajectory \
           \no: Execute trajectory:       \t\tp: Disable the motor \
           \nq: Exit                      \t\tr: Get mode')
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

    elif (selection == 'd'): # read the raw encoder value
        n_str = ser.read_until(b'\n');
        n_int = int(n_str)
        print('Encoder Degrees: ' + str(n_int) + '\n') # print it to the screen

    elif (selection == 'e'): # reset the raw encoder value
        print("Encoder has been reset to 32,768")

    elif (selection == 'f'): # Set PWM command
        n_str = input('Set PWM -100 to 100: ') # get the number to send
        print('Send PWM Command: ' + n_str) # print it to the screen to double check

        ser.write((n_str + '\n').encode())

    elif (selection == 'g'): # set current gains
        n_str1 = input('Current Cnt. Kp: ') # get the number to send
        n_str2 = input('Current Cnt. Ki: ') # get the number to send

        print("Sending gains, Kp = " + n_str1 + ", Ki = " + n_str2)

        ser.write((n_str1 + ' ' + n_str2 + '\n').encode())

    elif (selection == 'h'): # Get current gains
        n_str1 = ser.read_until(b'\n');
        n_str2 = ser.read_until(b'\n');

        n_f1 = float(n_str1)
        n_f2 = float(n_str2)

        print("Current gains, Kp = " + str(n_f1) + ", Ki = " + str(n_f2))

    elif (selection == 'i'): # Set position gains
        n_str1 = input('Position Cnt. Kp: ') # get the number to send
        n_str2 = input('Position Cnt. Ki: ') # get the number to send
        n_str3 = input('Position Cnt. Kd: ') # get the number to send

        print("Sending gains, Kp = " + n_str1 + ", Ki = " + n_str2 + " Kd = " + n_str3)

        ser.write((n_str1 + ' ' + n_str2 + ' ' + n_str3 + '\n').encode())

    elif (selection == 'j'): # Get position gains
        n_str1 = ser.read_until(b'\n');
        n_str2 = ser.read_until(b'\n');
        n_str3 = ser.read_until(b'\n');

        n_f1 = float(n_str1)
        n_f2 = float(n_str2)
        n_f3 = float(n_str3)

        print("Position gains, Kp = " + str(n_f1) + ", Ki = " + str(n_f2) + ", Kd = " + str(n_f3))

    elif (selection == 'k'): # Current gains testing

        sampnum = 0
        read_samples = 10
        act = []
        ref = []

        while sampnum < 99:
            data_read = ser.read_until(b'\n', 50)
            data_text = str(data_read, 'utf-8')
            data = list(map(int, data_text.split()))

            if(len(data) == 2):
                act.append(data[0])
                ref.append(data[1])
                sampnum = sampnum + 1

        t = range(len(act)) # time array
        plt.plot(t,act,'r*-',t,ref,'b*-')
        plt.ylabel('Current (mA)')
        plt.xlabel('Sample #')
        plt.title('Kp: ' + n_str1 + ', Ki: ' + n_str2)
        plt.show()

    elif (selection == 'l'):
        n_str1 = input('Enter an angle (0-360): ') # get the number to send
        ser.write((n_str1 + '\n').encode())

    elif (selection == 'p'): # disable motor
        print("Disabling Motor...")

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
