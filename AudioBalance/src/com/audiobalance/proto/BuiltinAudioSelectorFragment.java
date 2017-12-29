package com.audiobalance.proto;

import java.io.IOException;
import java.util.ArrayList;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.audiobalance.proto.AudioSelectorActivity.AudioSelectorFragment;

public class BuiltinAudioSelectorFragment extends AudioSelectorFragment {
    public static final String ARG_OBJECT = "object";

    @Override
    public View onCreateView(LayoutInflater inflater,
            ViewGroup container, Bundle savedInstanceState) {
        View rootView = inflater.inflate(
                R.layout.fragment_audiofileselector, container, false);
        
        ((TextView) rootView.findViewById(R.id.textview_audioselector_description)).setText(R.string.text_audioselector_builtin);
        
        String [] assetList = null;
        try {
        	assetList = getActivity().getResources().getAssets().list("");
        	Log.d("BuiltinAudioSelectorFragmnet", String.format("Loaded %d assets", assetList.length));
        } catch(IOException e) {
        	Log.e("BuiltinAudioSelectorFragment", "Error loading asset list");
        }
        
        ArrayList<MediaFile> fileList = new ArrayList<MediaFile>();

        for (int i = 0; i < assetList.length; i++) {
        	if (!assetList[i].endsWith(".mp3") && !assetList[i].endsWith(".wav")) {
        		continue;
        	}
        	MediaFile mf = MediaFile.create("asset:" + assetList[i], this.getActivity());
        	fileList.add(mf);
        }
        
        MediaFile [] fileArray = new MediaFile[fileList.size()];
        fileList.toArray(fileArray);
        CreateList(rootView, fileArray);
        
        return rootView;
    }
}	
