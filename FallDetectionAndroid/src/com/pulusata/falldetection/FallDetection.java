package com.pulusata.falldetection;


import java.util.ArrayList;

import android.app.ListActivity;
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
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Toast;


public class FallDetection extends ListActivity {
    private final String PREF_PHONE_NUMBERS = "phone_numbers";
    private final String PREF_NAMES = "names";
    protected Handler mHandler;
    ArrayList<String> contactList = new ArrayList<String>();
    ArrayList<String> nameList = new ArrayList<String>();
    ArrayAdapter<String> adapter;

    public void delete(View view) {
        while (contactList.size() != 0) {
            contactList.remove(contactList.size() - 1);
        }
        SharedPreferences phoneNumbers = getSharedPreferences(
                PREF_PHONE_NUMBERS, 0);
        phoneNumbers.edit().clear().commit();

        while (nameList.size() != 0) {
            nameList.remove(nameList.size() - 1);
        }
        SharedPreferences names = getSharedPreferences(PREF_NAMES, 0);
        names.edit().clear().commit();
        adapter.notifyDataSetChanged();

    }

    public void pickContact(View view) {
        Intent intent = new Intent(Intent.ACTION_PICK,
                Contacts.CONTENT_URI);
        intent.setType(ContactsContract.CommonDataKinds.Phone.CONTENT_TYPE);
        startActivityForResult(intent, 1);
    }

    protected void loadNames() {
        SharedPreferences names = getSharedPreferences(PREF_NAMES, 0);
        String currName;
        for (int i = 0; !((currName = names.getString(Integer.toString(i),
                "")).equals("")); i++) {
            nameList.add(currName);
        }
    }

    protected void loadNumbers() {
        SharedPreferences phoneNumbers = getSharedPreferences(
                PREF_PHONE_NUMBERS, 0);
        String currPhoneNumber;
        for (int i = 0; !((currPhoneNumber = phoneNumbers.getString(
                Integer.toString(i), "")).equals("")); i++) {
            contactList.add(currPhoneNumber);
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
                            contactList
                                    .add(PhoneNumberUtils
                                            .stripSeparators(formattedPhoneNumber));

                            Log.d("number", PhoneNumberUtils
                                    .stripSeparators(formattedPhoneNumber));

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
                        nameList.add(name);
                        adapter.notifyDataSetChanged();
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
        loadNumbers();
        loadNames();

        adapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_list_item_1, nameList);
        setListAdapter(adapter);

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        // Log.d("destroy", "destroy");
    }

    @Override
    protected void onListItemClick(ListView l, View v, int position,
            long id) {
        String item = (String) getListAdapter().getItem(position);
        Toast.makeText(this, item + " selected", Toast.LENGTH_LONG).show();
    }

    @Override
    protected void onPause() {
        super.onPause();
        saveNumbers();
        saveNames();
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    protected void saveNames() {
        SharedPreferences names = getSharedPreferences(PREF_NAMES, 0);
        SharedPreferences.Editor editor = names.edit();
        for (int i = 0; i < nameList.size(); i++) {
            String currName = nameList.get(i);
            editor.putString(Integer.toString(i), currName);
        }
        editor.commit();
    }

    protected void saveNumbers() {
        SharedPreferences phoneNumbers = getSharedPreferences(
                PREF_PHONE_NUMBERS, 0);
        SharedPreferences.Editor editor = phoneNumbers.edit();
        for (int i = 0; i < contactList.size(); i++) {
            String currPhoneNumber = contactList.get(i);
            editor.putString(Integer.toString(i), currPhoneNumber);
        }
        editor.commit();
    }

}
