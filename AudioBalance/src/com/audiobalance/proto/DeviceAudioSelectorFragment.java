package com.audiobalance.proto;

import java.util.ArrayList;

import android.database.Cursor;
import android.os.Bundle;
import android.provider.MediaStore;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.audiobalance.proto.AudioSelectorActivity.AudioSelectorFragment;

public class DeviceAudioSelectorFragment extends AudioSelectorFragment {
    public static final String ARG_OBJECT = "object";

    @Override
    public View onCreateView(LayoutInflater inflater,
            ViewGroup container, Bundle savedInstanceState) {
        View rootView = inflater.inflate(
                R.layout.fragment_audiofileselector, container, false);
        
        ((TextView) rootView.findViewById(R.id.textview_audioselector_description)).setText(R.string.text_audioselector_device);
        
        Cursor cursor = getActivity().getContentResolver().query(MediaStore.Audio.Media.EXTERNAL_CONTENT_URI, 
        		null, null, null, MediaStore.Audio.Media.TITLE + " ASC");
        Cursor videoCursor = getActivity().getContentResolver().query(MediaStore.Video.Media.EXTERNAL_CONTENT_URI, 
        		null, null, null, MediaStore.Audio.Media.TITLE + " ASC");
        
        ArrayList<MediaFile> fileList = new ArrayList<MediaFile>();
        
        addCursorToFileLst(cursor, fileList);
        addCursorToFileLst(videoCursor, fileList);
        
        MediaFile [] fileArray = new MediaFile[fileList.size()];
        fileList.toArray(fileArray);
        CreateList(rootView, fileArray);
        
        return rootView;
    }
    
    
    private void addCursorToFileLst(Cursor cursor, ArrayList<MediaFile> fileList) {
        int titleColumn = cursor.getColumnIndex(MediaStore.Audio.Media.TITLE);
        int artistColumn = cursor.getColumnIndex(MediaStore.Audio.Media.ARTIST);
        int durationColumn = cursor.getColumnIndex(MediaStore.Audio.Media.DURATION);
        int isNotificationColumn = cursor.getColumnIndex(MediaStore.Audio.Media.IS_NOTIFICATION);
        int isAlarmColumn = cursor.getColumnIndex(MediaStore.Audio.Media.IS_ALARM);
        int isRingtoneColumn = cursor.getColumnIndex(MediaStore.Audio.Media.IS_RINGTONE);
        int dataColumn = cursor.getColumnIndex(MediaStore.Audio.Media.DATA);

        while(cursor.moveToNext()) {
        	MediaFile fileEntry = new MediaFile();
        	fileEntry.name = cursor.getString(titleColumn);
        	fileEntry.artist = cursor.getString(artistColumn);
        	fileEntry.duration = cursor.getInt(durationColumn);
        	fileEntry.isAlarm = isAlarmColumn >= 0 ? cursor.getInt(isAlarmColumn) != 0 : false;
        	fileEntry.isNotification = isNotificationColumn >= 0 ? cursor.getInt(isNotificationColumn) != 0 : false;
        	fileEntry.isRingtone = isRingtoneColumn >= 0 ? cursor.getInt(isRingtoneColumn) != 0 : false;
        	fileEntry.path = cursor.getString(dataColumn);
        	fileList.add(fileEntry);
        }
    	
    }
    
}
