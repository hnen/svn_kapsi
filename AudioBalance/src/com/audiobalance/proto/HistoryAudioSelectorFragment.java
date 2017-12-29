package com.audiobalance.proto;

import java.util.ArrayList;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.audiobalance.proto.AudioSelectorActivity.AudioSelectorFragment;

public class HistoryAudioSelectorFragment extends AudioSelectorFragment {
    public static final String ARG_OBJECT = "object";

    @Override
    public View onCreateView(LayoutInflater inflater,
            ViewGroup container, Bundle savedInstanceState) {
        View rootView = inflater.inflate(
                R.layout.fragment_audiofileselector, container, false);

        ((TextView) rootView.findViewById(R.id.textview_audioselector_description)).setText(R.string.text_audioselector_history);
        
        ArrayList<MediaFile> fileList = new ArrayList<MediaFile>();
        String [] history = PlayHistoryManager.get(this.getActivity());

        for (int i = 0; i < history.length; i++) {
        	MediaFile mf = MediaFile.create(history[i], this.getActivity());
        	fileList.add(mf);
        }
        
        MediaFile [] fileArray = new MediaFile[fileList.size()];
        fileList.toArray(fileArray);
        CreateList(rootView, fileArray);
        
        return rootView;
    }
}	
