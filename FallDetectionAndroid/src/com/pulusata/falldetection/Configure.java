package com.pulusata.falldetection;


import java.util.UUID;

import android.app.Activity;
import android.content.SharedPreferences;
import android.content.res.Configuration;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.RadioGroup;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;
import android.widget.Toast;

import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.util.PebbleDictionary;


public class Configure extends Activity {

    private SeekBar tresholdBar;
    private String PREF_CONFIG = "configuration";
    int tresholdLevel;
    private String currentMessage;

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
    }

    public void save(View view) {
        SharedPreferences config = getSharedPreferences(PREF_CONFIG, 0);
        SharedPreferences.Editor editor = config.edit();

        editor.putString("msg",
                ((EditText) findViewById(R.id.customMessage)).getText()
                        .toString());

        editor.commit();
        ((TextView) findViewById(R.id.current_message))
                .setText("Current Message: "
                        + ((EditText) findViewById(R.id.customMessage))
                                .getText().toString());
    }

    public void sendDataToWatch() {
        // Build up a Pebble dictionary
        PebbleDictionary data = new PebbleDictionary();

        data.addUint8(0, (byte) tresholdLevel);

        // Send the assembled dictionary to the weather watch-app;
        PebbleKit.sendDataToPebble(getApplicationContext(),
                UUID.fromString("5ed10362-a625-41e6-b35c-e6b10feb71e6"),
                data);
    }

    protected void loadConfig() {
        SharedPreferences config = getSharedPreferences(PREF_CONFIG, 0);
        tresholdLevel = Integer.parseInt(config.getString("tresholdValue",
                "50"));
        currentMessage = config.getString("msg", "Help Me!");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_configure);
        loadConfig();
        tresholdBar = (SeekBar) findViewById(R.id.treshold);
        RadioGroup tresholdGroup = (RadioGroup) findViewById(R.id.treshold_defaults);

        tresholdBar
                .setOnSeekBarChangeListener(new OnSeekBarChangeListener() {

                    @Override
                    public void onProgressChanged(SeekBar seekBar,
                            int progress, boolean fromUser) {
                        tresholdLevel = progress;
                        ((TextView) findViewById(R.id.fall_treshold_display))
                                .setText("Fall Treshold: " + tresholdLevel);
                    }

                    @Override
                    public void onStartTrackingTouch(SeekBar seekBar) {
                    }

                    @Override
                    public void onStopTrackingTouch(SeekBar seekBar) {

                        saveTreshold();

                        Toast.makeText(Configure.this,
                                "seek bar progress:" + tresholdLevel,
                                Toast.LENGTH_SHORT).show();
                        tresholdBar.post(new Runnable() {

                            @Override
                            public void run() {
                                tresholdBar
                                        .setSecondaryProgress(tresholdLevel);
                                sendDataToWatch();

                            }

                        });
                    }
                });
        tresholdGroup
                .setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {

                    @Override
                    public void onCheckedChanged(RadioGroup group,
                            int checkedId) {
                        switch (checkedId) {
                        case R.id.young:
                            tresholdLevel = 65;
                            break;
                        case R.id.above60F:
                            tresholdLevel = 40;
                            break;
                        case R.id.above60M:
                            tresholdLevel = 45;
                            break;
                        case R.id.car_crash:
                            tresholdLevel = 70;
                            break;
                        case R.id.osteoporosis:
                            tresholdLevel = 40;
                            break;
                        case R.id.osteopenia:
                            tresholdLevel = 45;
                            break;
                        case R.id.less12:
                            tresholdLevel = 55;
                            break;
                        case R.id.fractured:
                            tresholdLevel = 40;
                            break;
                        case R.id.default_tresh:
                            tresholdLevel = 50;
                            break;
                        default:
                            break;
                        }
                        saveTreshold();
                        tresholdBar.post(new Runnable() {

                            @Override
                            public void run() {
                                tresholdBar
                                        .setSecondaryProgress(tresholdLevel);
                                tresholdBar.setProgress(tresholdLevel);
                                sendDataToWatch();

                            }

                        });
                    }

                });

    }

    @Override
    protected void onResume() {
        super.onResume();
        tresholdBar.setProgress(tresholdLevel);
        tresholdBar.setSecondaryProgress(tresholdLevel);
        ((TextView) findViewById(R.id.current_message))
                .setText("Current Message: " + currentMessage);
    }

    protected void saveTreshold() {
        SharedPreferences config = getSharedPreferences(PREF_CONFIG, 0);
        SharedPreferences.Editor editor = config.edit();
        editor.putString("tresholdValue", Integer.toString(tresholdLevel));
        editor.commit();
    }
}
