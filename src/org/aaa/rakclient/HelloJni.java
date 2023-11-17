/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package org.aaa.rakclient;

import android.app.Activity;
import android.widget.TextView;
import android.os.Bundle;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.widget.Toast;
import android.widget.EditText;
import android.widget.Button;
import android.widget.TextView;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.app.ProgressDialog;
import android.text.InputType;
import android.view.View;

import java.util.List;
import java.util.ArrayList;



import org.apache.commons.codec.net.*;
import android.widget.*;
import android.widget.Magnifier.*;
import android.provider.Settings.Secure;
public class HelloJni extends Activity
{
    /** Called when the activity is first created. */
    @Override
	public String textStatus = "";
	public boolean dialogOpen = false;
	public ListAdapter globalAdapter;
	public ListView viewL;
	private AlertDialog.Builder builder;
	private AlertDialog builderDialog;
	//private ProgressDialog currProgressDiag;
	private String android_id = "nonid";
	public List<String> stringList = new ArrayList<String>();

	@Override
	protected void onPause()
	{
		//dismiss dialog, crash bad
		if (dialogOpen) {
			dialogOpen = false;
			builderDialog.dismiss();
		}
		super.onPause();
	}
	
	public String getTheFuckingId() {
		return android_id;
	}
	
	private void refreshData() {
		runOnUiThread(new Runnable() {

				@Override
				public void run() {

					// Stuff that updates the ui
				    int index = viewL.getFirstVisiblePosition();
					View v = viewL.getChildAt(0);
					int top = (v == null) ? 0 : (v.getTop() - viewL.getPaddingTop());
		            globalAdapter = new ArrayAdapter<String>(org.aaa.rakclient.HelloJni.this, android.R.layout.simple_list_item_1, stringList);

		            viewL.setAdapter(globalAdapter);
					
					viewL.setSelectionFromTop(index, top);
				}
			});
	}
	
	public void lostConnection() {
		runOnUiThread(new Runnable() {

				@Override
				public void run() {
		final TextView wtf = (TextView)findViewById(R.id.connStat);
		wtf.setText("Status: Disconnected");
		}
		
		});
	}
	
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        /* Create a TextView and set its content.
         * the text is retrieved by calling a native
         * function.
         */
		 
		android_id = Secure.getString(getApplicationContext().getContentResolver(),Secure.ANDROID_ID); //first thing we will do is set the id
		 
        setContentView(R.layout.main);
		
		//nativeInit();
		
		final Thread thread = new Thread(){
			public void run(){
				nativeInit();
			    startConnectionNative();
			}
		};
		
		inputHandler();
		
		viewL = (ListView)findViewById(R.id.logList);
		
		refreshData();
		
		final Button btnWtf = (Button)findViewById(R.id.send_message);
		
		btnWtf.setOnClickListener(new View.OnClickListener(){
			public void onClick(View what) {
				EditText msg_edit = (EditText)findViewById(R.id.message_text);

				String msg = msg_edit.getText().toString();

				sendMessageNative(msg);
				
				stringList.add("Your message: "+msg);
				refreshData();
				
				msg_edit.setText("");
			}
		});
		
		thread.start();
    }

    /* A native method that is implemented by the
     * 'hello-jni' native library, which is packaged
     * with this application.
     */
    public native String  stringFromJNI();
	
	public native void nativeInit();
	
	public native void setClientUsernameNative(String name);
	
	public native void sendMessageNative(String test);
	
	public native void startConnectionNative();
	
	public native void stopConnectionNative();

    /* This is another native method declaration that is *not*
     * implemented by 'hello-jni'. This is simply to show that
     * you can declare as many native methods in your Java code
     * as you want, their implementation is searched in the
     * currently loaded native libraries only the first time
     * you call them.
     *
     * Trying to call this function will result in a
     * java.lang.UnsatisfiedLinkError exception !
     */
    public native String  unimplementedStringFromJNI();

	public String inputHandler() {
		if(dialogOpen) {
			dialogOpen = true;
			builder = new AlertDialog.Builder(HelloJni.this);
			builder.setTitle("Select username");

// Set up the input
			final EditText input = new EditText(this);

// Specify the type of input expected; this, for example, sets the input as a password, and will mask the text
			input.setInputType(InputType.TYPE_CLASS_TEXT);
			builder.setView(input);

// Set up the buttons
			builder.setPositiveButton("OK", new DialogInterface.OnClickListener() { 
					@Override
					public void onClick(DialogInterface dialog, int which) {
						textStatus = input.getText().toString();
						setClientUsernameNative(textStatus);
						dialogOpen = false;
					}
				});
			builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface dialog, int which) {
						dialog.cancel();
						textStatus = "ee";
						dialogOpen = false;
					}
				});

			builderDialog = builder.show();
		}
		return "ee";
	}
	
	public void warn(String what) {
		stringList.add("Warning: "+what);
		refreshData();
	}
	
	public void tell(String what) {
		stringList.add("Message: "+what);
		refreshData();
	}
	
    /* this is used to load the 'hello-jni' library on application
     * startup. The library has already been unpacked into
     * /data/data/com.example.hellojni/lib/libhello-jni.so at
     * installation time by the package manager.
     */
    static {
        System.loadLibrary("main");
    }
}
