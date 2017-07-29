package clicnet.smartcat;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;

import org.eclipse.paho.client.mqttv3.MqttException;

import static clicnet.smartcat.MainActivity.client;
import static clicnet.smartcat.MainActivity.topic1;

public class CatManager extends AppCompatActivity {

    EditText timeCat1;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_cat_manager);

        timeCat1=(EditText)findViewById(R.id.editTextCat1Time);


    }


    public void setCat1Time(View v){
        String topic = topic1;
        String payload = "timecat1-"+timeCat1.getText().toString();
        try {
            client.publish(topic, payload.getBytes(),0,false);
        } catch ( MqttException e) {
            e.printStackTrace();
        }
    }
}
