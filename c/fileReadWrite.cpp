extern "C" {
  #include "libavformat/avformat.h"
}

int open_input_file(char *file,
                           AVFormatContext **input_format_context)
{
    AVCodecContext *avctx;
    int error;

    /* Open the input file to read from it. */
    if ((error = avformat_open_input(input_format_context, file, NULL,
                                     NULL)) < 0) {
        fprintf(stderr, "Could not open input file '%s' (error '%s')\n",
                file, av_err2str(error));
        *input_format_context = NULL;
        return error;
    }

    /* Get information on the input file (number of streams etc.). */
    if ((error = avformat_find_stream_info(*input_format_context, NULL)) < 0) {
        fprintf(stderr, "Could not open find stream info (error '%s')\n",
                av_err2str(error));
        avformat_close_input(input_format_context);
        return error;
    }

    return 0;
}
