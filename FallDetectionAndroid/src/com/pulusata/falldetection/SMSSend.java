package com.pulusata.falldetection;


import java.util.List;
import java.util.UUID;

import android.content.Context;
import android.content.SharedPreferences;
import android.location.Address;
import android.location.Criteria;
import android.location.Geocoder;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.telephony.SmsManager;
import android.text.format.DateUtils;

import com.getpebble.android.kit.PebbleKit;
import com.getpebble.android.kit.PebbleKit.PebbleDataReceiver;
import com.getpebble.android.kit.util.PebbleDictionary;


public class SMSSend extends PebbleDataReceiver {

    private static final UUID PEBBLE_APP_UUID = UUID
            .fromString("5ed10362-a625-41e6-b35c-e6b10feb71e6");
    private final String PREF_PHONE_NUMBERS = "phone_numbers";
    private final String message = "Help me!";
    private LocationManager mLocationManager;
    private Context mContext;

    private final LocationListener mLocationListener = new LocationListener() {
        @Override
        public void onLocationChanged(Location location) {
            sendSMS(location, false);
            mLocationManager.removeUpdates(this);
        }

        @Override
        public void onProviderDisabled(String provider) {
        }

        @Override
        public void onProviderEnabled(String provider) {
        }

        @Override
        public void onStatusChanged(String provider, int status,
                Bundle bundle) {
        }
    };

    public SMSSend() {
        super(PEBBLE_APP_UUID);
    }

    @Override
    public void receiveData(Context context, int transactionId,
            PebbleDictionary data) {
        PebbleKit.sendAckToPebble(context, transactionId);

        mContext = context;
        mLocationManager = (LocationManager) context
                .getSystemService(Context.LOCATION_SERVICE);

        Criteria criteria = new Criteria();
        criteria.setAccuracy(Criteria.ACCURACY_FINE);
        criteria.setCostAllowed(false);

        String mProviderName = mLocationManager.getBestProvider(criteria,
                true);

        Location lastLocation = mLocationManager
                .getLastKnownLocation(mProviderName);
        if (data.getInteger(1).intValue() == 42) {

            if (lastLocation == null
                    || (System.currentTimeMillis()
                            - lastLocation.getTime() > DateUtils.MINUTE_IN_MILLIS * 10)) {
                mLocationManager.requestLocationUpdates(mProviderName,
                        10000, 10, mLocationListener);
                sendSMS(lastLocation, true);
            } else if ((System.currentTimeMillis()
                    - lastLocation.getTime() < DateUtils.MINUTE_IN_MILLIS * 10)) {
                sendSMS(lastLocation, false);
            }

        }

    }

    protected void sendSMS(Location location, boolean last) {
        Geocoder geocoder = new Geocoder(mContext);
        String addressText = "";

        try {
            List<Address> addresses = geocoder.getFromLocation(
                    location.getLatitude(), location.getLongitude(), 1);

            if (addresses.size() > 0) {
                Address a = addresses.get(0);
                addressText = "Address: ";
                for (int i = 0; i <= a.getMaxAddressLineIndex(); i++) {
                    addressText += a.getAddressLine(i) + " ";
                }
            }
        } catch (Exception e) {
            // unable to geocode
            addressText = "";
        }

        String locationString;
        if (location != null && !last) {
            locationString = " New location: " + location.getLatitude()
                    + ", " + location.getLongitude() + " ";
        } else if (last) {
            locationString = " Last known location: "
                    + location.getLatitude() + ", "
                    + location.getLongitude() + " ";
        } else {
            locationString = "Waiting for location";
        }

        SharedPreferences phoneNumbers = mContext.getSharedPreferences(
                PREF_PHONE_NUMBERS, 0);
        String currPhoneNumber;

        for (int i = 0; !((currPhoneNumber = phoneNumbers.getString(
                Integer.toString(i), "")).equals("")); i++) {
            try {
                SmsManager smsManager = SmsManager.getDefault();
                smsManager.sendTextMessage(currPhoneNumber, null, message
                        + locationString + addressText, null, null);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

    }
}
