package com.pulusata.falldetection;


import android.app.Activity;
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
import android.util.Log;
import android.view.View;
import android.widget.LinearLayout;
import android.widget.TextView;


public class FallDetection extends Activity {
    private LinearLayout phoneNumLayout;
    // private String message = "Help Me!!";
    private final String PREF_PHONE_NUMBERS = "phone_numbers";
    protected Handler mHandler;

    // public void broadcast(View view) {
    // sendBroadcast();
    // }

    public void delete(View view) {
        phoneNumLayout.removeAllViews();
        SharedPreferences phoneNumbers = getSharedPreferences(
                PREF_PHONE_NUMBERS, 0);
        phoneNumbers.edit().clear().commit();

    }

    public void pickContact(View view) {
        Intent intent = new Intent(Intent.ACTION_PICK,
                Contacts.CONTENT_URI);
        intent.setType(ContactsContract.CommonDataKinds.Phone.CONTENT_TYPE);
        startActivityForResult(intent, 1);
    }

    protected void loadNumbers() {
        SharedPreferences phoneNumbers = getSharedPreferences(
                PREF_PHONE_NUMBERS, 0);
        String currPhoneNumber;
        for (int i = 0; !((currPhoneNumber = phoneNumbers.getString(
                Integer.toString(i), "")).equals("")); i++) {
            TextView phoneNumber = new TextView(this);
            phoneNumber.setText(currPhoneNumber);
            phoneNumLayout.addView(phoneNumber);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode,
            Intent data) {

        if (resultCode == RESULT_OK) {
            if (data != null && requestCode == 1) {
                Uri uriOfPhoneNumberRecord = data.getData();
                String idOfPhoneRecord = uriOfPhoneNumberRecord
                        .getLastPathSegment();
                Cursor cursor = getContentResolver().query(
                        Phone.CONTENT_URI, new String[] { Phone.NUMBER },
                        Phone._ID + "=?",
                        new String[] { idOfPhoneRecord }, null);
                if (cursor != null) {
                    if (cursor.getCount() > 0) {
                        cursor.moveToFirst();
                        String formattedPhoneNumber = cursor
                                .getString(cursor
                                        .getColumnIndex(Phone.NUMBER));
                        if (formattedPhoneNumber != null) {
                            // Set phone number
                            TextView phoneNumber = new TextView(this);
                            phoneNumber
                                    .setText(PhoneNumberUtils
                                            .stripSeparators(formattedPhoneNumber));
                            phoneNumLayout.addView(phoneNumber);
                        }
                    }
                    cursor.close();
                }

                Cursor c = getContentResolver().query(
                        uriOfPhoneNumberRecord, null, null, null, null);
                if (c != null) {
                    if (c.moveToFirst()) {
                        String name = c
                                .getString(c
                                        .getColumnIndex(ContactsContract.Contacts.DISPLAY_NAME));
                        Log.d("name", name);
                    }
                }
            }
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_fall_detection);
        phoneNumLayout = (LinearLayout) findViewById(R.id.phone_numbers);
        loadNumbers();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        // Log.d("destroy", "destroy");
    }

    @Override
    protected void onPause() {
        super.onPause();
        saveNumbers();
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    protected void saveNumbers() {
        SharedPreferences phoneNumbers = getSharedPreferences(
                PREF_PHONE_NUMBERS, 0);
        SharedPreferences.Editor editor = phoneNumbers.edit();
        for (int i = 0; i < phoneNumLayout.getChildCount(); i++) {
            String currPhoneNumber = ((TextView) (phoneNumLayout
                    .getChildAt(i))).getText().toString();
            editor.putString(Integer.toString(i), currPhoneNumber);
        }
        editor.commit();
    }

    // protected void sendBroadcast() {
    // try {
    // SmsManager smsManager = SmsManager.getDefault();
    // for (int i = 0; i < phoneNumLayout.getChildCount(); i++) {
    // String currPhoneNumber = ((TextView) (phoneNumLayout
    // .getChildAt(i))).getText().toString();
    // smsManager.sendTextMessage(currPhoneNumber, null, message,
    // null, null);
    // }
    // } catch (Exception e) {
    // e.printStackTrace();
    // }
    // }

}
