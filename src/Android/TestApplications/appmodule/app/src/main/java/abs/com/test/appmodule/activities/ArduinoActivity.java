package abs.com.test.appmodule.activities;


import abs.com.test.appmodule.R;
import abs.com.test.appmodule.services.TestService.LocalBinder;
import abs.com.test.appmodule.services.TestService;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

public class ArduinoActivity extends Activity
{

    public TestService mService;
    public boolean mBound = false;

    /**
     * Variable to manage the service binding
     */
    private ServiceConnection mConnection;


    public ArduinoActivity()
    {
        /* Initialize the binding manager variable overriding some of the
        ServiceConnection class methods */
        mConnection = new ServiceConnection()
        {
            /**
             * Called when the connection with the service is established
             * @param className
             * @param service
             */
            @Override
            public void onServiceConnected(ComponentName className,
                                           IBinder service)
            {
                /* Because we have bound to an explicit service that is running
                in our own process, we can cast its IBinder to a concrete class
                and directly access it */
                mService = ((LocalBinder) service).getService();
                mBound = true;
            }

            /**
             * Called when the connection with the service disconnects
             * unexpectedly
             * @param className
             */
            @Override
            public void onServiceDisconnected(ComponentName className) {
                mBound = false;
            }
        };
    }

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_arduino);

        /* Start the service on creating the activity */
        Intent service = new Intent(ArduinoActivity.this, TestService.class);
        startService(service);

        /* Bind to the service so that we can interact with it */
        bindService(service, mConnection, Context.BIND_AUTO_CREATE);
    }

    @Override
    protected void onStart()
    {
        super.onStart();
    }

    @Override
    protected void onStop()
    {
        super.onStop();
        if(mBound)
        {
            unbindService(mConnection);
            mBound = false;
        }
    }

    /**
     * Called when the check button is clicked
     * @param v
     */
    public void onButtonClick(View v)
    {
        /* If the service is bound... */
        if (mBound) {
            /* Call a method from the LocalService. However, if this call were
            something that might hang, then this request should occur in a
            separate thread to avoid slowing down the activity performance. */
            mService.runTest("arduino");
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu)
    {
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item)
    {
        int id = item.getItemId();

        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

}