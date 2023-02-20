package com.waveshare.epaperesp32loader.image_processing

import android.graphics.Bitmap
import android.graphics.Color
import com.waveshare.epaperesp32loader.AppStartActivity
import com.waveshare.epaperesp32loader.image_processing.EPaperPicture
import com.waveshare.epaperesp32loader.image_processing.EPaperDisplay

/**
 * <h1>Image filtering for e-Paper display</h1>
 * The class is intended for pixel data converting
 * from ARGB format to e-Paper display suitable format.
 * The result of the pixel format's converting is an index
 * of color in e-Paper dislay's palette
 *
 * @author  Waveshare team
 * @version 1.0
 * @since   8/14/2018
 */
object EPaperPicture {
    private var srcW = 0
    private var srcH // Width and height of source image
            = 0
    private var dstW = 0
    private var dstH // Width and height of destination image
            = 0
    private var srcBmp // Bitmap of source image
            : Bitmap? = null
    private var dstBmp // Bitmap of destination image
            : Bitmap? = null
    private var curPal // Current palette
            = intArrayOf()
    private val palettes = arrayOf(
        intArrayOf(Color.GRAY, Color.WHITE),
        intArrayOf(Color.BLACK, Color.WHITE, Color.RED),
        intArrayOf(
            Color.BLACK, Color.WHITE, Color.GRAY
        ),
        intArrayOf(Color.BLACK, Color.WHITE, Color.GRAY, Color.RED),
        intArrayOf(
            Color.BLACK, Color.WHITE
        ),
        intArrayOf(Color.BLACK, Color.WHITE, Color.YELLOW),
        intArrayOf(),
        intArrayOf(-0x1000000, -0x1, -0xff0100, -0xffff01, -0x10000, -0x100, -0x8000)
    )

    // Return the square error of {r, g, b},
    // that means how far them are from standard color stdCol
    //---------------------------------------------------------
    private fun getErr(r: Double, g: Double, b: Double, stdCol: Int, count: Int = 1): Double {
        var r = r
        var g = g
        var b = b
        r -= Color.red(stdCol).toDouble()
        g -= Color.green(stdCol).toDouble()
        b -= Color.blue(stdCol).toDouble()
        return r * r + g * g + b * b * count
    }

    // Return the index of current palette color which is
    // nearest to the {r, g, b}
    //---------------------------------------------------------
    private fun getNear(r: Double, g: Double, b: Double, count: Int = 1): Int {
        var ind = 0
        var err = getErr(r, g, b, curPal[0], count)
        for (i in 1 until curPal.size) {
            val cur = getErr(r, g, b, curPal[i])
            if (cur < err) {
                err = cur
                ind = i
            }
        }
        return ind
    }

    // Return the index of current palette color which is
    // nearest to the color clr
    //---------------------------------------------------------
    private fun getNear(clr: Int, count: Int = 1): Int {
        return getNear(
            Color.red(clr).toDouble(),
            Color.green(clr).toDouble(),
            Color.blue(clr).toDouble(),
            count
        )
    }

    // Adding of color {r, g, b} into e color array with
    // weight k. Here every r, g or b channel takes one cell
    // in e color array and can have any integer value.
    //---------------------------------------------------------
    private fun addVal(e: DoubleArray?, i: Int, r: Double, g: Double, b: Double, k: Double) {
        val index = i * 3
        e!![index] = r * k / 16 + e[index]
        e[index + 1] = g * k / 16 + e[index + 1]
        e[index + 2] = b * k / 16 + e[index + 2]
    }

    // Returns a color from the current palette
    // which is nearest to source bitmap pixel at (x, y), or
    // returns default color if (x, y) is out of the bitmap
    //---------------------------------------------------------
    private fun nearColor(x: Int, y: Int, count: Int = 1): Int {
        return if (x >= srcW || y >= srcH) curPal[if ((x + y) % 2 == 0) 1 else 0] else curPal[
                getNear(srcBmp!!.getPixel(x, y), count)
        ]
    }

    // Returns bitmap of pixels from current palette.
    //
    // isLvl = true - the method of searching the nearest color
    // is based on estimation how less the original pixel
    // differs from the palette's pixels.
    //
    // isLvl = false - the method of searching the nearest color
    // is based on estimation how less the original pixel's area
    // differs from the palette's pixels (so called dithering).
    //---------------------------------------------------------
    fun createIndexedImage(isLvl: Boolean, isRed: Boolean, count: Int = 1): Bitmap? {
        val epd = EPaperDisplay.getDisplays()[EPaperDisplay.epdInd]
        srcBmp = AppStartActivity.originalImage
        dstBmp = Bitmap.createBitmap(epd.width, epd.height, srcBmp!!.getConfig())
        var palInd = epd.index
        if (!isRed) palInd = palInd and 0xE
        curPal = palettes[palInd]
        dstW = dstBmp!!.getWidth()
        dstH = dstBmp!!.getHeight()
        srcW = srcBmp!!.getWidth()
        srcH = srcBmp!!.getHeight()
        val srcArr = IntArray(srcW * srcH)
        val dstArr = IntArray(dstW * dstH)
        var index = 0
        srcBmp!!.getPixels(srcArr, 0, srcW, 0, 0, srcW, srcH)
        if (isLvl) {
            for (y in 0 until dstH) for (x in 0 until dstW) dstArr[index++] = nearColor(x, y, count)
        } else {
            var aInd = 0
            var bInd = 1
            val errArr = arrayOfNulls<DoubleArray>(2)
            errArr[0] = DoubleArray(3 * dstW)
            errArr[1] = DoubleArray(3 * dstW)
            for (i in 0 until dstW) {
                errArr[bInd]!![3 * i] = 0.0
                errArr[bInd]!![3 * i + 1] = 0.0
                errArr[bInd]!![3 * i + 2] = 0.0
            }
            for (j in 0 until dstH) {
                if (j >= srcH) {
                    var i = 0
                    while (i < dstW) {
                        dstArr[index] = curPal[if ((i + j) % 2 == 0) 1 else 0]
                        i++
                        index++
                    }
                    continue
                }
                aInd = aInd.also { bInd = it } + 1 and 1
                for (i in 0 until dstW) {
                    errArr[bInd]!![3 * i] = 0.0
                    errArr[bInd]!![3 * i + 1] = 0.0
                    errArr[bInd]!![3 * i + 2] = 0.0
                }
                for (i in 0 until dstW) {
                    if (i >= srcW) {
                        dstArr[index++] = curPal[if ((i + j) % 2 == 0) 1 else 0]
                        continue
                    }
                    val srcPix = srcArr[j * srcW + i]
                    var r = Color.red(srcPix) + errArr[aInd]!![3 * i]
                    var g = Color.green(srcPix) + errArr[aInd]!![3 * i + 1]
                    var b = Color.blue(srcPix) + errArr[aInd]!![3 * i + 2]
                    val colVal = curPal[getNear(r, g, b)]
                    dstArr[index++] = colVal
                    r -= Color.red(colVal).toDouble()
                    g -= Color.green(colVal).toDouble()
                    b -= Color.blue(colVal).toDouble()
                    if (i == 0) {
                        addVal(errArr[bInd], i, r, g, b, 7.0)
                        addVal(errArr[bInd], i + 1, r, g, b, 2.0)
                        addVal(errArr[aInd], i + 1, r, g, b, 7.0)
                    } else if (i == dstW - 1) {
                        addVal(errArr[bInd], i - 1, r, g, b, 7.0)
                        addVal(errArr[bInd], i, r, g, b, 9.0)
                    } else {
                        addVal(errArr[bInd], i - 1, r, g, b, 3.0)
                        addVal(errArr[bInd], i, r, g, b, 5.0)
                        addVal(errArr[bInd], i + 1, r, g, b, 1.0)
                        addVal(errArr[aInd], i + 1, r, g, b, 7.0)
                    }
                }
            }
        }

        // Put converted pixels into destination image bitmap
        //-----------------------------------------------------
        dstBmp!!.setPixels(dstArr, 0, dstW, 0, 0, dstW, dstH)
        return dstBmp
    }
}