package com.waveshare.epaperesp32loader.image_processing;

/**
 * Created by YiWan on 8/24/2018.
 */

/**
 * <h1>e-Paper display characteristics</h1>
 * The class is intended to store width, height and index of palette.
 *
 * @author  Waveshare team
 * @version 1.0
 * @since   8/14/2018
 */

public class EPaperDisplay
{
    public int    width;
    public int    height;
    public int    index;
    public String title;

    /**
     * Stores display's characteristics.
     * @param width of display in pixels
     * @param height of display on pixels
     * @param index of palette
     * @param title of display
     */
    public EPaperDisplay(int width, int height, int index, String title)
    {
        this.width  = width;
        this.height = height;
        this.index  = index;
        this.title  = title;
    }

    // Index of selected display
    //---------------------------------------------------------
    public static int epdInd = -1;

    // Array of display characteristics
    //---------------------------------------------------------
    private static EPaperDisplay[] array = null;

    public static EPaperDisplay[] getDisplays()
    {
        if (array == null)
        {
            array = new EPaperDisplay[]
            {
                new EPaperDisplay(176,264,0, "2.7 inch e-Paper V2")         // 42
            };
        }
        return array;
    }
}