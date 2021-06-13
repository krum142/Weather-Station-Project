package com.example.tangra;

import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.os.Bundle;
import android.widget.Button;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;



import com.hoho.android.usbserial.driver.UsbSerialDriver;
import com.hoho.android.usbserial.driver.UsbSerialPort;
import com.hoho.android.usbserial.driver.UsbSerialProber;
import com.hoho.android.usbserial.util.SerialInputOutputManager;

import org.jetbrains.annotations.NotNull;


import java.io.IOException;
import java.util.List;

import java.util.concurrent.Executors;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.FormBody;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;


public class MainActivity extends AppCompatActivity implements SerialInputOutputManager.Listener {

    TextView arduinoTextView;
    TextView recivedHttpView;
    TextView connectedView;
    Button button;


    private enum UsbPermission { Unknown, Requested, Granted, Denied };
    private UsbPermission usbPermission = UsbPermission.Unknown;
    private SerialInputOutputManager usbIoManager;
    private Context context;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        context = this;
        button = findViewById(R.id.button);
        arduinoTextView = findViewById(R.id.ArduinotextView);
        recivedHttpView = findViewById(R.id.RecivedHttpView);
        connectedView = findViewById(R.id.ConnectedView);



        button.setOnClickListener(v -> {
           connect();
            //getRequest();

        });
    }

    public void postRequest(String number){
        String url = "http://46.55.200.199:25565/WeatherForecast";
        OkHttpClient client = new OkHttpClient();

        RequestBody formbody = new FormBody.Builder()
                .add("x",number)
                .build();

        Request request = new Request.Builder()
                .url(url)
                .post(formbody)
                .build();

        client.newCall(request).enqueue(new Callback() {
            @Override
            public void onFailure(@NotNull Call call, @NotNull IOException e) {
                recivedHttpView.setText(e.getMessage());
            }

            @Override
            public void onResponse(@NotNull Call call, @NotNull Response response) throws IOException {
                if(response.isSuccessful()){
                    recivedHttpView.setText(response.body().string());
                }
            }
        });
    }

    @Override
    public void onNewData(byte[] data) {
        runOnUiThread(() -> {
            arduinoTextView.append(new String(data));
            //postRequest(new String(data));
            postRequest(new String(data));
        });
    }

    @Override
    public void onRunError(Exception e) {

    }


    private void connect() {
        UsbDevice device = null;
        UsbManager usbManager = (UsbManager) getSystemService(Context.USB_SERVICE);

        List<UsbSerialDriver> avalableDrivers = UsbSerialProber.getDefaultProber().findAllDrivers(usbManager);
        if(avalableDrivers.size() == 0){
            connectedView.setText("No Avalable drivers");
            return;
        }
        UsbSerialDriver driver = avalableDrivers.get(0);
        UsbDeviceConnection usbConnection = usbManager.openDevice(driver.getDevice());

        UsbSerialPort usbSerialPort = driver.getPorts().get(0);
        if(usbConnection == null && usbPermission == UsbPermission.Unknown && !usbManager.hasPermission(driver.getDevice())) {
            usbPermission = UsbPermission.Requested;
            PendingIntent usbPermissionIntent = PendingIntent.getBroadcast(this, 0, new Intent(BuildConfig.APPLICATION_ID + ".GRANT_USB"), 0);
            usbManager.requestPermission(driver.getDevice(), usbPermissionIntent);
            return;
        }
        if(usbConnection == null) {
            if (!usbManager.hasPermission(driver.getDevice())){
                connectedView.setText("connection failed: permission denied");
            }
            else{
                connectedView.setText("connection failed: open failed");
            }
            return;
        }

        try {
            usbSerialPort.open(usbConnection);
            usbSerialPort.setParameters(9600, 8, 1, UsbSerialPort.PARITY_NONE);

            usbIoManager = new SerialInputOutputManager(usbSerialPort, this);
            Executors.newSingleThreadExecutor().submit(usbIoManager);

            connectedView.setText("Port Opened !");

        } catch (Exception e) {
            connectedView.setText("Last Error");
        }
    }
}