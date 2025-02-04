#include "encoder.h"

#include <stdint.h>
#include "hardware/gpio.h"
#include "pins.h"
#include "wheels.h"

#define CM_PER_NOTCH 1.05
#define TIMEOUT_THRESHOLD 1500000

static int leftStopCounter = 0;
static int rightStopCounter = 0;

// Global variables to store measurement data for the left wheel
volatile uint32_t leftNotchCount = 0;
volatile double leftTotalDistance = 0.0;
volatile uint64_t leftLastNotchTime = 0;

// Global variables to store measurement data for the right wheel
volatile uint32_t rightNotchCount = 0;
volatile double rightTotalDistance = 0.0;
volatile uint64_t rightLastNotchTime = 0;

// Timer for reseting encoder values after a period of idleness
struct repeating_timer encoderTimer;

// Function to print current encoder data for both wheels
static inline void printEncoderData(void) {
    printf("Left Wheel - Notch Count: %u, Distance: %.4f cm, Speed: %.4f cm/s\n",
           leftNotchCount, leftTotalDistance, pid_left.current_speed);
    printf("Right Wheel - Notch Count: %u, Distance: %.4f cm, Speed: %.4f cm/s\n",
           rightNotchCount, rightTotalDistance, pid_right.current_speed);
}

// This timer is used to set detected speed to zero if too much time has passed without movement
bool encoderTimerCallback(struct repeating_timer *t)
{
    uint64_t currentTime = time_us_64();
    // If encoder hasn't changed after the set time, set speed to zero
    if (currentTime - leftLastNotchTime > TIMEOUT_THRESHOLD)
        pid_left.current_speed = 0.f;
    if (currentTime - rightLastNotchTime > TIMEOUT_THRESHOLD)
        pid_right.current_speed = 0.f;
    return true;
}

// Combined encoder callback to handle both left and right encoder interrupts
void encoderCallback(uint gpio, uint32_t events) {
    uint64_t currentTime = time_us_64();

    if (gpio == WHEEL_ENCODER_LEFT_PIN) {
        // Calculate time difference and speed for the left wheel
        uint64_t timeDiff = currentTime - leftLastNotchTime;
        if (timeDiff > 0 && timeDiff < TIMEOUT_THRESHOLD) {           
            // Increment the count of notches detected for the left wheel
            leftNotchCount++;
            leftTotalDistance = (double)leftNotchCount * CM_PER_NOTCH;
            pid_left.current_speed = CM_PER_NOTCH / (timeDiff / 1e6);
            
        } else {
            pid_left.current_speed = 0.0;
        }

        leftLastNotchTime = currentTime;
    } else if (gpio == WHEEL_ENCODER_RIGHT_PIN) {

        // Calculate time difference and speed for the right wheel
        // If time diff is larger than acceptable threshold, assume right encoder was stopped
        uint64_t timeDiff = currentTime - rightLastNotchTime;
        if (timeDiff > 0  && timeDiff < TIMEOUT_THRESHOLD) {     
            // Increment the count of notches detected for the right wheel
            rightNotchCount++;
            rightTotalDistance = (double)rightNotchCount * CM_PER_NOTCH;
            pid_right.current_speed = CM_PER_NOTCH / (timeDiff / 1e6);
        } else {
            pid_right.current_speed = 0.0;
        }

        rightLastNotchTime = currentTime;
    }
}

// Function to check if the car has stopped and set speed to zero if no movement
void checkIfStopped() {
    uint64_t currentTime = time_us_64();

    if (currentTime - leftLastNotchTime > TIMEOUT_THRESHOLD) {
        leftStopCounter++;
    } else {
        leftStopCounter = 0;
    }

    if (currentTime - rightLastNotchTime > TIMEOUT_THRESHOLD) {
        rightStopCounter++;
    } else {
        rightStopCounter = 0;
    }

    // Only set speed to zero if the counter exceeds a threshold which is 3 checks in a row
    if (leftStopCounter >= 3) {
        // pid_left.current_speed = 0.f;
    }

    if (rightStopCounter >= 3) {
        // pid_right.current_speed = 0.f;
    }
}

// Setup function for the encoder pins and interrupts
void setupEncoderPins() {
    gpio_init(WHEEL_ENCODER_LEFT_PIN);
    gpio_set_dir(WHEEL_ENCODER_LEFT_PIN, GPIO_IN);

    gpio_init(WHEEL_ENCODER_RIGHT_PIN);
    gpio_set_dir(WHEEL_ENCODER_RIGHT_PIN, GPIO_IN);

    add_repeating_timer_ms(100, encoderTimerCallback, NULL, &encoderTimer);
}

// Resets the internal variables used to track distance travelled by encoder
void resetEncoder() {
    leftNotchCount = 0;
    leftTotalDistance = 0.0;
    leftLastNotchTime = time_us_64();

    rightNotchCount = 0;
    rightTotalDistance = 0.0;
    rightNotchCount = time_us_64();
}