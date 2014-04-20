package com.pulusata.falldetection;

import java.util.UUID;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.provider.ContactsContract;
import android.provider.ContactsContract.CommonDataKinds.Phone;
import android.provider.ContactsContract.Contacts;
import android.telephony.PhoneNumberUtils;
import android.telephony.SmsManager;
import android.util.Log;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.PebbleKit.PebbleDataReceiver;
import com.getpebble.android.kit.util.PebbleDictionary;

public class FallDetection extends Activity {

    private static final UUID PEBBLE_APP_UUID = UUID.fromString("5ed10362-a625-41e6-b35c-e6b10feb71e6");
	private LinearLayout phoneNumLayout;
    private String message="Help Me!!";
    private final String PREF_PHONE_NUMBERS= "phone_numbers";
	private PebbleDataReceiver dataReceiver;
	protected Handler mHandler; 

	@Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_fall_detection);
        phoneNumLayout=(LinearLayout)findViewById(R.id.phone_numbers);
        loadNumbers();
    }
    public void broadcast(View view){
    	sendBroadcast();
    }
    public void pickContact(View view) {
        Intent intent = new Intent(Intent.ACTION_PICK, Contacts.CONTENT_URI);
        intent.setType(ContactsContract.CommonDataKinds.Phone.CONTENT_TYPE);
        startActivityForResult(intent, 1);
    }
    public void delete(View view){
    	phoneNumLayout.removeAllViews();
    	SharedPreferences phoneNumbers=getSharedPreferences(PREF_PHONE_NUMBERS, 0);
    	phoneNumbers.edit().clear().commit();
    	
    }
    protected void sendBroadcast(){
    	try {
            SmsManager smsManager = SmsManager.getDefault();
            for(int i=0;i<phoneNumLayout.getChildCount();i++){
            	String currPhoneNumber=((TextView)(phoneNumLayout.getChildAt(i))).getText().toString(); 
            	smsManager.sendTextMessage(currPhoneNumber, null, message, null,
            			null);
            }
//            Toast.makeText(getApplicationContext(), "SMS Sent!",
//                    Toast.LENGTH_LONG).show();
        } catch (Exception e) {
//            Toast.makeText(getApplicationContext(),
//                    "SMS faild, please try again later!", Toast.LENGTH_LONG)
//                    .show();
            e.printStackTrace();
        }
    }
    protected void saveNumbers(){
    	SharedPreferences phoneNumbers=getSharedPreferences(PREF_PHONE_NUMBERS, 0);
    	SharedPreferences.Editor editor= phoneNumbers.edit();
    	for(int i=0;i<phoneNumLayout.getChildCount();i++){
    		String currPhoneNumber=((TextView)(phoneNumLayout.getChildAt(i))).getText().toString();
    		editor.putString(Integer.toString(i), currPhoneNumber);
    	}
    	editor.commit();
    }
    protected void loadNumbers(){
    	SharedPreferences phoneNumbers=getSharedPreferences(PREF_PHONE_NUMBERS, 0);
    	String currPhoneNumber;
    	for(int i=0;!((currPhoneNumber=phoneNumbers.getString(Integer.toString(i),"")).equals(""));i++){
    		TextView phoneNumber = new TextView(this);
    		phoneNumber.setText(currPhoneNumber);
    		phoneNumLayout.addView(phoneNumber);
    	}
    }
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (resultCode == RESULT_OK) {
            if(data != null && requestCode == 1) {
                Uri uriOfPhoneNumberRecord = data.getData();
                String idOfPhoneRecord = uriOfPhoneNumberRecord.getLastPathSegment();
                Cursor cursor = getContentResolver().query(Phone.CONTENT_URI, new String[]{Phone.NUMBER}, Phone._ID + "=?", new String[]{idOfPhoneRecord}, null);
                if(cursor != null) {
                    if(cursor.getCount() > 0) {
                        cursor.moveToFirst();
                        String formattedPhoneNumber = cursor.getString( cursor.getColumnIndex(Phone.NUMBER) );
                        if (formattedPhoneNumber != null) {
                            TextView phoneNumber = new TextView(this);
                            phoneNumber.setText(PhoneNumberUtils.stripSeparators(formattedPhoneNumber));
                            phoneNumLayout.addView(phoneNumber);
                        }
                    }
                    cursor.close();
                }
            }
        }
    }
    @Override
    protected void onPause() {
        super.onPause();
        if (dataReceiver != null) {
            unregisterReceiver(dataReceiver);
            dataReceiver = null;
        }
        saveNumbers();
    }
    protected void onResume() {
        super.onResume();
        dataReceiver = new PebbleKit.PebbleDataReceiver(PEBBLE_APP_UUID) {
            @Override
            public void receiveData(final Context context, final int transactionId, final PebbleDictionary data) {
                final int cmd = data.getInteger(1).intValue();
                Log.d("cmd",""+cmd);
                if(cmd==42){
                	sendBroadcast();
                }
                PebbleKit.sendAckToPebble(context, transactionId);
            }
        };
        PebbleKit.registerReceivedDataHandler(this, dataReceiver);
    }

}
