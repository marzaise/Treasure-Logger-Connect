package com.waveshare.epaperesp32loader

import android.support.v7.app.AppCompatActivity
import android.widget.TextView
import android.os.Bundle
import com.waveshare.epaperesp32loader.R
import com.waveshare.epaperesp32loader.image_processing.EPaperDisplay
import android.content.Intent
import android.app.Activity
import android.view.View
import android.view.Window
import android.widget.Button
import android.widget.ImageView
import android.widget.SeekBar
import android.widget.SeekBar.OnSeekBarChangeListener
import com.waveshare.epaperesp32loader.image_processing.EPaperPicture

/**
 * <h1>Filtering activity</h1>
 * The activity offers to select one of available image filters,
 * which converts the loaded image for better pixel format
 * converting required for selected display.
 *
 * @author  Waveshare team
 * @version 1.0
 * @since   8/18/2018
 */
class FilteringActivity : AppCompatActivity() {
    // View
    //------------------------------------------
    private var button: Button? = null
    private var textView: TextView? = null
    private var imageView: ImageView? = null
    override fun onCreate(savedInstanceState: Bundle?) {
        requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS)
        super.onCreate(savedInstanceState)
        setContentView(R.layout.palettes_activity)
        supportActionBar!!.setTitle(R.string.filt)

        // View
        //------------------------------------------
        textView = findViewById(R.id.txt_indexed)
        imageView = findViewById(R.id.img_indexed)

        // Disable unavailable palettes
        //------------------------------------------
        var redIsEnabled = EPaperDisplay.getDisplays()[EPaperDisplay.epdInd].index and 1 != 0
        findViewById<View>(R.id.btn_wbrl).isEnabled = redIsEnabled
        findViewById<View>(R.id.btn_wbrd).isEnabled = redIsEnabled

        // Disable unavailable palettes just for 5.65f e-Paper
        //------------------------------------------
        if (EPaperDisplay.epdInd == 25 || EPaperDisplay.epdInd == 37) {
            redIsEnabled = false
            findViewById<View>(R.id.btn_wb_l).isEnabled = redIsEnabled
            findViewById<View>(R.id.btn_wb_d).isEnabled = redIsEnabled
        }

        findViewById<SeekBar>(R.id.seekbar).setOnSeekBarChangeListener(object: OnSeekBarChangeListener{
            override fun onProgressChanged(p0: SeekBar?, p1: Int, p2: Boolean) {
                run(true, false, (p1-50)/10)
            }

            override fun onStartTrackingTouch(p0: SeekBar?) {
            }

            override fun onStopTrackingTouch(p0: SeekBar?) {
            }

        })
    }

    // Accept the selected
    fun onOk(view: View?) {
        // If palette is not selected, then exit
        //-----------------------------------------------------
        if (button == null) return

        // Close palette activity and return palette's name
        //-----------------------------------------------------
        val intent = Intent()
        intent.putExtra("NAME", button!!.text.toString())
        setResult(RESULT_OK, intent)
        finish()
    }

    fun onCancel(view: View?) {
        onBackPressed()
    }

    override fun onBackPressed() {
        setResult(RESULT_CANCELED)
        finish()
    }

    fun onWhiteAndBlackLevelClick(view: View?) {
        // Save pushed button and run image processing
        //-----------------------------------------------------
        button = view as Button?
        run(true, false)
    }

    fun onWhiteBlackRedLevelClick(view: View?) {
        // Save pushed button and run image processing
        //-----------------------------------------------------
        button = view as Button?
        run(true, true)
    }

    fun onWhiteAndBlackDitheringClick(view: View?) {
        // Save pushed button and run image processing
        //-----------------------------------------------------
        button = view as Button?
        run(false, false)
    }

    fun onWhiteBlackRedDitheringClick(view: View?) {
        // Save pushed button and run image processing
        //-----------------------------------------------------
        button = view as Button?
        run(false, true)
    }

    fun run(isLvl: Boolean, isRed: Boolean, count: Int = 1) {
        // Image processing
        //-----------------------------------------------------
        AppStartActivity.indTableImage = EPaperPicture.createIndexedImage(isLvl, isRed, count)

        // Image view size calculation
        //-----------------------------------------------------
        val size = textView!!.width
        imageView!!.maxHeight = size
        imageView!!.minimumHeight = size / 2
        imageView!!.setImageBitmap(AppStartActivity.indTableImage)

        // Show selected image filter
        //-----------------------------------------------------
        textView!!.text = button!!.text
    }
}