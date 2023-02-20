package com.waveshare.epaperesp32loader

import android.support.v7.app.AppCompatActivity
import android.widget.AdapterView.OnItemClickListener
import com.waveshare.epaperesp32loader.communication.PermissionHelper
import com.waveshare.epaperesp32loader.OpenFileActivity.ReadPermissionResponse
import android.widget.TextView
import com.waveshare.epaperesp32loader.OpenFileActivity.FileListAdapter
import android.os.Bundle
import com.waveshare.epaperesp32loader.R
import android.os.Environment
import android.app.Activity
import android.content.Context
import android.content.Intent
import android.view.View
import android.widget.AdapterView
import android.widget.ArrayAdapter
import android.view.ViewGroup
import android.view.Window
import android.widget.ListView
import com.waveshare.epaperesp32loader.communication.PermissionHelper.PermissionResponse
import java.io.File
import java.util.*

/**
 * <h1>Open file activity</h1>
 * The activity if a simple file browser.
 * It provides the file choosing within the external storage.
 *
 * @author  Waveshare team
 * @version 1.0
 * @since   8/17/2018
 */
class OpenFileActivity : AppCompatActivity(), OnItemClickListener {
    // Permission
    //--------------------------------------
    private var permissionHelper: PermissionHelper? = null
    private var permissionResponse: ReadPermissionResponse? = null

    // Views
    //--------------------------------------
    var textView: TextView? = null
    var listView: ListView? = null

    // Files
    //--------------------------------------
    var thisFolder: File? = null
    var externalStorage: String? = null

    // File list
    //--------------------------------------
    var fileArrayList: ArrayList<File>? = null
    var fileListAdapter: FileListAdapter? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS)
        super.onCreate(savedInstanceState)
        setContentView(R.layout.open_file_activity)
        supportActionBar!!.setTitle(R.string.file)

        // Permission (READ_EXTERNAL_STORAGE)
        //--------------------------------------
        permissionHelper = PermissionHelper(this)
        permissionResponse = ReadPermissionResponse()
        permissionHelper!!.setResponse(PermissionHelper.REQ_READ, permissionResponse)

        // Views
        //--------------------------------------
        textView = findViewById(R.id.open_file_text)
        listView = findViewById(R.id.open_file_list)

        // File list adapter
        //--------------------------------------
        fileArrayList = ArrayList()
        fileListAdapter = FileListAdapter(this, fileArrayList)
        listView?.setAdapter(fileListAdapter)
        listView?.setOnItemClickListener(this)

        // Set current folder
        //--------------------------------------
        externalStorage = Environment.getExternalStorageDirectory().absolutePath
        val fileName = AppStartActivity.filePath
        thisFolder =
            if (fileName == null || !fileName.startsWith(externalStorage!!)) Environment.getExternalStorageDirectory() else File(
                fileName
            )
        select(thisFolder)
    }

    fun select(folder: File?) {
        // Set folder's file into the permission's response
        //-----------------------------------------------------
        permissionResponse!!.file = folder

        // Check permission and run response if it's granted
        //-----------------------------------------------------
        if (permissionHelper!!.sendRequestPermission(PermissionHelper.REQ_READ)) permissionResponse!!.invoke()
    }

    // On coming to parent folder event handler
    //---------------------------------------------------------
    fun onBack(view: View?) {
        // Exit if the external storage root is current folder
        //-----------------------------------------------------
        if (thisFolder!!.absolutePath ==
            Environment.getExternalStorageDirectory().absolutePath
        ) return

        // Setup parent folder
        //-----------------------------------------------------
        thisFolder = thisFolder!!.parentFile
        textView!!.text = thisFolder?.getPath()

        // Update list of files
        //-----------------------------------------------------
        fileArrayList!!.clear()
        fileArrayList!!.addAll(Arrays.asList(*thisFolder?.listFiles()))
        fileListAdapter!!.notifyDataSetChanged()
    }

    fun onCancel(view: View?) {
        onBackPressed()
    }

    override fun onBackPressed() {
        setResult(RESULT_CANCELED, Intent())
        finish()
    }

    override fun onItemClick(adapterView: AdapterView<*>?, view: View, i: Int, l: Long) {
        select(fileListAdapter!!.getItem(i))
    }

    //---------------------------------------------------------
    //  File list adapter
    //---------------------------------------------------------
    inner class FileListAdapter(context: Context?, files: ArrayList<File>?) :
        ArrayAdapter<File?>(context, android.R.layout.simple_list_item_1, files?.toList()) {
        override fun getView(position: Int, convertView: View, parent: ViewGroup): View {
            // Get list item view and its file
            //-------------------------------------------------
            val view = super.getView(position, convertView, parent) as TextView
            val file = getItem(position)

            // Put the name of the file into its view
            //-------------------------------------------------
            if (view != null) view.text = file!!.name
            return view
        }
    }

    //------------------------------------------
    //  Result of permission request
    //------------------------------------------
    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<String>,
        grantResults: IntArray
    ) {
        permissionHelper!!.onRequestPermissionsResult(requestCode, permissions, grantResults)
    }

    //---------------------------------------------------------
    //  Permission responses
    //---------------------------------------------------------
    internal inner class ReadPermissionResponse : PermissionResponse {
        var file: File? = null
        override fun invoke() {
            // Check if the file is located in external storage
            //-------------------------------------------------
            if (file == null || !file!!.absolutePath.startsWith(externalStorage!!)) file =
                Environment.getExternalStorageDirectory()

            // Case of file: return its name and path
            //-------------------------------------------------
            if (file!!.isFile) {
                AppStartActivity.fileName = file!!.name
                AppStartActivity.filePath = thisFolder!!.absolutePath
                setResult(RESULT_OK, Intent())
                finish()
            } else {
                var message = "Current location: "
                message += file!!.absolutePath.substring(externalStorage!!.length)
                textView!!.text = message

                // Get items of the folder
                //---------------------------------------------
                val files = file!!.listFiles() ?: return

                // Mark the folder as current one
                //---------------------------------------------
                thisFolder = file

                // Update file list view
                //---------------------------------------------
                fileArrayList!!.clear()
                fileArrayList!!.addAll(Arrays.asList(*files))
                fileListAdapter!!.notifyDataSetChanged()
            }
        }
    }
}