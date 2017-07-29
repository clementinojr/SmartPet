package clicnet.smartcat;

import android.content.Context;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

public class MainActivity extends AppCompatActivity {

    Button btn;

    static String MQTTHOST = "tcp://m12.cloudmqtt.com:19337";
    static String USERNAME = "dmsjvruj";
    static String PASSWORD = "miXaqhWnTVA9";
    static String topic1 = "esp8266/pincmd";//sent by mobile or site
    static String topic2 = "esp8266/up";//sent by esp8266

    static MqttAndroidClient client;


    TextView statusText;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        establishMQTT();

        statusText = (TextView)findViewById(R.id.textStatus);


        callBack();

        buttonCatManager();
    }



    public void buttonCatManager() {
        final Context context = this;
        btn = (Button) findViewById(R.id.buttonCatManager);
        btn.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View arg0) {
                Intent intent = new Intent(context, CatManager.class);
                startActivity(intent);
            }

        });

    }
    public void callBack(){
    client.setCallback(new MqttCallback() {
        @Override
        public void connectionLost(Throwable cause) {

        }

        @Override
        public void messageArrived(String topic, MqttMessage message) throws Exception {

//            String tempTopic = topic;
//            statusText.setText(topic);
            if (topic.equals(topic2)) {
                if (new String(message.getPayload()).contains("open")) {
                statusText.setText("Aberto");
                } else {
                statusText.setText("Fechado");
                }
            }
        }

        @Override
        public void deliveryComplete(IMqttDeliveryToken token) {

        }
    });

    }

    public void establishMQTT(){
        String clientId = MqttClient.generateClientId();
        client = new MqttAndroidClient(this.getApplicationContext(), MQTTHOST, clientId);

        MqttConnectOptions options = new MqttConnectOptions();

        options.setUserName(USERNAME);
        options.setPassword(PASSWORD.toCharArray());



        try {
            IMqttToken token = client.connect(options);
            token.setActionCallback(new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    // We are connected
                    Log.d("connection", "onSuccess");

                    setStatus();
                }

                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    // Something went wrong e.g. connection timeout or firewall problems
                    Log.d("connection", "onFailure");

                }
            });
        } catch (MqttException e) {
            e.printStackTrace();
        }

    }

    public void open(View v){
        String topic = topic1;
        String payload = "open";
        try {
            client.publish(topic, payload.getBytes(),0,false);
        } catch ( MqttException e) {
            e.printStackTrace();
        }
    }
    public void close(View v){
        String topic = topic1;
        String payload = "close";
        try {
            client.publish(topic, payload.getBytes(),0,false);
        } catch ( MqttException e) {
            e.printStackTrace();
        }
    }




    private void setStatus(){
        try{
            client.subscribe(topic2,0);
        }catch (MqttException e){
            e.printStackTrace();
        }


    }
}
