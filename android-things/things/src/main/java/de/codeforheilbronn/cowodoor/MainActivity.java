package de.codeforheilbronn.cowodoor;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import com.google.android.things.pio.Gpio;
import com.google.android.things.pio.GpioCallback;
import com.google.android.things.pio.PeripheralManagerService;

import java.io.IOException;

/**
 * Skeleton of an Android Things activity.
 * <p>
 * Android Things peripheral APIs are accessible through the class
 * PeripheralManagerService. For example, the snippet below will open a GPIO pin and
 * set it to HIGH:
 * <p>
 * <pre>{@code
 * PeripheralManagerService service = new PeripheralManagerService();
 * mLedGpio = service.openGpio("BCM6");
 * mLedGpio.setDirection(Gpio.DIRECTION_OUT_INITIALLY_LOW);
 * mLedGpio.setValue(true);
 * }</pre>
 * <p>
 * For more complex peripherals, look for an existing user-space driver, or implement one if none
 * is available.
 *
 * @see <a href="https://github.com/androidthings/contrib-drivers#readme">https://github.com/androidthings/contrib-drivers#readme</a>
 */


public class MainActivity extends Activity {
    private TextView doorText;
    private Gpio gpio;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        doorText = findViewById(R.id.doorText);
        doorText.setText("Warte auf Sensor");
        initSensor();
    }

    private void initSensor() {
        PeripheralManagerService service = new PeripheralManagerService();
        try {
            Log.d("Door", "initSensor: "+service.getGpioList());
            gpio = service.openGpio("GPIO_32"); //Bei preview 6+ GPIO2_IO00
            configureInput(gpio);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public void configureInput(Gpio gpio) throws IOException {
        // Initialize the pin as an input
        gpio.setDirection(Gpio.DIRECTION_IN);
        // Low voltage is considered active
        gpio.setActiveType(Gpio.ACTIVE_LOW);

        // Register for all state changes
        gpio.setEdgeTriggerType(Gpio.EDGE_BOTH);
        gpio.registerGpioCallback(mGpioCallback);
    }

    private GpioCallback mGpioCallback = new GpioCallback() {
        @Override
        public boolean onGpioEdge(Gpio gpio) {
            // Read the active low pin state
            Log.d("Door", "onGpioEdge: ");
            try {
                if (gpio.getValue()) {
                    doorText.setText("Tür offen");
                } else {
                    doorText.setText("Tür zu");
                }
            } catch (IOException e) {
                e.printStackTrace();
            }

            // Continue listening for more interrupts
            return true;
        }

        @Override
        public void onGpioError(Gpio gpio, int error) {
            Log.w("Door", gpio + ": Error event " + error);
        }
    };

}
