using Android.App;
using Android.Widget;
using Android.OS;
using System;
using uPLibrary.Networking.M2Mqtt;
using System.Net;
using uPLibrary.Networking.M2Mqtt.Messages;
using System.Text;
using System.Security.Cryptography.X509Certificates;

namespace GatoAppV1
{
    [Activity(Label = "GatoAppV1", MainLauncher = true)]
    public class MainActivity : Activity
    {
        Button button1;
        TextView txtCampo;
        protected override void OnCreate(Bundle savedInstanceState)
        {

            // register to message received 
            //client.MqttMsgPublishReceived += client_MqttMsgPublishReceived;

            //string clientId = Guid.NewGuid().ToString();
            //client.Connect(clientId);

            // subscribe to the topic "/home/temperature" with QoS 2 
            //client.Subscribe(new string[] { "/home/temperature" }, new byte[] { MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE });



            base.OnCreate(savedInstanceState);

            // Set our view from the "main" layout resource
            SetContentView(Resource.Layout.Main);
            button1 = FindViewById<Button>(Resource.Id.button1);
            txtCampo = FindViewById<TextView>(Resource.Id.txtCampo);
            button1.Click += BntEscrever;

            //button.Click+= delegate { button.Text=string.Format("{0}click
        }
        static void client_MqttMsgPublishReceived(object sender, MqttMsgPublishEventArgs e)
        {
            // handle message received 
        }

        void BntEscrever(object sender , EventArgs e)
        {


            // create client instance 
             X509Certificate caCert = new X509Certificate();
             X509Certificate    caCert2 = new X509Certificate();
             MqttSslProtocols mqttSslProtocols = new MqttSslProtocols();
             
             MqttClient client = new MqttClient("m12.cloudmqtt.com", 19337,false,caCert,caCert2, mqttSslProtocols);



            

            string clientId = Guid.NewGuid().ToString();
            client.Connect(clientId, "dmsjvruj", "miXaqhWnTVA9");

            string strValue = Convert.ToString("tetetet");
            // publish a message on "/home/temperature" topic with QoS 2 
            client.Publish("/home/temperature", Encoding.UTF8.GetBytes(strValue), MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE, false);
            txtCampo.Text = "TUDO SIM LINDO";
        }
    }
}

