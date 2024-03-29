/* Play MP3 file from flash

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>

#include "esp_log.h"

#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "audio_error.h"
#include "tone_stream.h"
#include "audio_mem.h"
#include "audio_common.h"
#include "i2s_stream.h"
// #include "mp3_decoder.h"
#include "wav_decoder.h"

#include "esp_peripherals.h"
#include "periph_touch.h"
#include "periph_adc_button.h"
#include "periph_button.h"

#include "board.h"

#if __has_include("audio_tone_uri.h")
    #include "audio_tone_uri.h"
#else
    #error "please refer the README, and then make the tone file"
#endif

static const char *TAG = "TONE_WAV_EXAMPLE";
static int MUSIC = 0;

void app_main(void)
{
    audio_pipeline_handle_t pipeline;
    audio_element_handle_t tone_stream_reader, i2s_stream_writer, wav_decoder;

    esp_log_level_set("*", ESP_LOG_WARN);
    esp_log_level_set(TAG, ESP_LOG_INFO);

    ESP_LOGI(TAG, "[ 1 ] Start codec chip");
    audio_board_handle_t board_handle = audio_board_init();
    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_DECODE, AUDIO_HAL_CTRL_START);

    int player_volume;
    audio_hal_get_volume(board_handle->audio_hal, &player_volume);

    ESP_LOGI(TAG, "[2.0] Create audio pipeline for playback");
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);
    AUDIO_NULL_CHECK(TAG, pipeline, return);

    ESP_LOGI(TAG, "[2.1] Create tone stream to read data from flash");
    tone_stream_cfg_t tone_cfg = TONE_STREAM_CFG_DEFAULT();
    tone_cfg.type = AUDIO_STREAM_READER;
    tone_stream_reader = tone_stream_init(&tone_cfg);
    AUDIO_NULL_CHECK(TAG, tone_stream_reader, return);

    ESP_LOGI(TAG, "[2.2] Create i2s stream to write data to codec chip");
    i2s_stream_cfg_t i2s_cfg = I2S_STREAM_CFG_DEFAULT();
    i2s_cfg.type = AUDIO_STREAM_WRITER;
    i2s_stream_writer = i2s_stream_init(&i2s_cfg);
    AUDIO_NULL_CHECK(TAG, i2s_stream_writer, return);

    // ESP_LOGI(TAG, "[2.3] Create mp3 decoder to decode mp3 file");
    // mp3_decoder_cfg_t mp3_cfg = DEFAULT_MP3_DECODER_CONFIG();
    // mp3_decoder = mp3_decoder_init(&mp3_cfg);
    // AUDIO_NULL_CHECK(TAG, mp3_decoder, return);
   
    ESP_LOGI(TAG, "[2.3] Create WAV decoder to decode WAV file");
    wav_decoder_cfg_t wav_cfg = DEFAULT_WAV_DECODER_CONFIG();
    wav_decoder = wav_decoder_init(&wav_cfg);
    AUDIO_NULL_CHECK(TAG, wav_decoder, return);

    ESP_LOGI(TAG, "[2.4] Register all elements to audio pipeline");
    audio_pipeline_register(pipeline, tone_stream_reader, "tone");
    audio_pipeline_register(pipeline, wav_decoder, "wav");
    audio_pipeline_register(pipeline, i2s_stream_writer, "i2s");

    ESP_LOGI(TAG, "[2.5] Link it together [flash]-->tone_stream-->wav_decoder-->i2s_stream-->[codec_chip]");
    const char *link_tag[3] = {"tone", "wav", "i2s"};
    audio_pipeline_link(pipeline, &link_tag[0], 3);

    ESP_LOGI(TAG, "[2.6] Set up  uri (file as tone_stream, wav as wav decoder, and default output is i2s)");
    audio_element_set_uri(tone_stream_reader, tone_uri[MUSIC]);

    ESP_LOGI(TAG, "[ 3 ] Initialize peripherals");
    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);

    ESP_LOGI(TAG, "[3.1] Initialize keys on board");
    audio_board_key_init(set);

    ESP_LOGI(TAG, "[ 4 ] Set up event listener");
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    audio_event_iface_handle_t evt = audio_event_iface_init(&evt_cfg);

    ESP_LOGI(TAG, "[4.1] Listening event from all elements of pipeline");
    audio_pipeline_set_listener(pipeline, evt);

    ESP_LOGI(TAG, "[4.2] Listening event from peripherals");
    audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt);

    ESP_LOGW(TAG, "[ 5 ] Tap touch buttons to control music player:");
    ESP_LOGW(TAG, "      [Play] to start, pause and resume, [Set] to stop.");
    ESP_LOGW(TAG, "      [Vol-] or [Vol+] to adjust volume.");

    ESP_LOGI(TAG, "[ 6 ] Listen for all pipeline events");
    while (1) {
        // audio_event_iface_msg_t msg = { 0 };
        audio_event_iface_msg_t msg;
        esp_err_t ret = audio_event_iface_listen(evt, &msg, portMAX_DELAY);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "[ * ] Event interface error : %d", ret);
            continue;
        }

        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) wav_decoder
            && msg.cmd == AEL_MSG_CMD_REPORT_MUSIC_INFO) {
            audio_element_info_t music_info = {0};
            audio_element_getinfo(wav_decoder, &music_info);

            ESP_LOGI(TAG, "[ * ] Receive music info from wav decoder, sample_rates=%d, bits=%d, ch=%d",
                     music_info.sample_rates, music_info.bits, music_info.channels);

            audio_element_setinfo(i2s_stream_writer, &music_info);
            i2s_stream_set_clk(i2s_stream_writer, music_info.sample_rates, music_info.bits, music_info.channels);
            continue;
        }

        if ((msg.source_type == PERIPH_ID_TOUCH || msg.source_type == PERIPH_ID_BUTTON || msg.source_type == PERIPH_ID_ADC_BTN)
            && (msg.cmd == PERIPH_TOUCH_TAP || msg.cmd == PERIPH_BUTTON_PRESSED || msg.cmd == PERIPH_ADC_BUTTON_PRESSED)) {
            if ((int) msg.data == get_input_play_id()) {
                ESP_LOGI(TAG, "[ * ] [Play] touch tap event");
                audio_element_state_t el_state = audio_element_get_state(i2s_stream_writer);
                switch (el_state) {
                    case AEL_STATE_INIT :
                        ESP_LOGI(TAG, "[ * ] Starting audio pipeline");
                        audio_pipeline_run(pipeline);
                        break;
                    case AEL_STATE_RUNNING :
                        ESP_LOGI(TAG, "[ * ] Pausing audio pipeline");
                        audio_pipeline_pause(pipeline);
                        break;
                    case AEL_STATE_PAUSED :
                        ESP_LOGI(TAG, "[ * ] Resuming audio pipeline");
                        audio_pipeline_resume(pipeline);
                        break;
                    case AEL_STATE_FINISHED :
                        ESP_LOGI(TAG, "[ * ] Rewinding audio pipeline");
                        audio_pipeline_reset_ringbuffer(pipeline);
                        audio_pipeline_reset_elements(pipeline);
                        audio_pipeline_change_state(pipeline, AEL_STATE_INIT);
                        audio_pipeline_run(pipeline);
                        break;
                    default :
                        ESP_LOGI(TAG, "[ * ] Not supported state %d", el_state);
                }
            } else if ((int) msg.data == get_input_set_id()) {
                ESP_LOGI(TAG, "[ * ] [Set] touch tap event");
                ESP_LOGI(TAG, "[ * ] Stopping audio pipeline");
                break;
            } else if ((int) msg.data == get_input_volup_id()) {
                ESP_LOGI(TAG, "[ * ] [Vol+] touch tap event");
                player_volume += 10;
                if (player_volume > 100) {
                    player_volume = 100;
                }
                audio_hal_set_volume(board_handle->audio_hal, player_volume);
                ESP_LOGI(TAG, "[ * ] Volume set to %d %%", player_volume);
            } else if ((int) msg.data == get_input_voldown_id()) {
                ESP_LOGI(TAG, "[ * ] [Vol-] touch tap event");
                player_volume -= 10;
                if (player_volume < 0) {
                    player_volume = 0;
                }
                audio_hal_set_volume(board_handle->audio_hal, player_volume);
                ESP_LOGI(TAG, "[ * ] Volume set to %d %%", player_volume);
            }else if ((int) msg.data == get_input_rec_id()) {
                ESP_LOGI(TAG, "[ * ] [BACK] touch REC event");
                ESP_LOGI(TAG, "[ * ] BACK to last pipeline");
                if(MUSIC>0){
                    MUSIC--;
                    ESP_LOGI(TAG, "the current song has been changed into: %d",MUSIC);
                }else{
                    MUSIC=0;
                } 
                audio_element_set_uri(tone_stream_reader, tone_uri[MUSIC]);
            }
            else if ((int) msg.data == get_input_mode_id()) {
                ESP_LOGI(TAG, "[ * ] [NEXT] touch Mode event");
                ESP_LOGI(TAG, "[ * ] next to forma pipeline");
                if(MUSIC<2){
                    MUSIC++;
                    ESP_LOGI(TAG, "the current song has been changed into: %d",MUSIC);
                }else{
                    MUSIC=2;
                } 
                audio_element_set_uri(tone_stream_reader, tone_uri[MUSIC]);
            }
        }

        // /* Stop when the last pipeline element (i2s_stream_writer in this case) receives stop event */
        // if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) i2s_stream_writer
        //     && msg.cmd == AEL_MSG_CMD_REPORT_STATUS
        //     && (((int)msg.data == AEL_STATUS_STATE_STOPPED) || ((int)msg.data == AEL_STATUS_STATE_FINISHED))) {
        //     ESP_LOGW(TAG, "[ * ] Stop event received");
        //     break;
        // }
    }

    ESP_LOGI(TAG, "[ 7 ] Stop audio_pipeline");
    audio_pipeline_stop(pipeline);
    audio_pipeline_wait_for_stop(pipeline);
    audio_pipeline_terminate(pipeline);

    audio_pipeline_unregister(pipeline, tone_stream_reader);
    audio_pipeline_unregister(pipeline, i2s_stream_writer);
    audio_pipeline_unregister(pipeline, wav_decoder);

    /* Terminal the pipeline before removing the listener */
    audio_pipeline_remove_listener(pipeline);

    /* Make sure audio_pipeline_remove_listener & audio_event_iface_remove_listener are called before destroying event_iface */
    audio_event_iface_destroy(evt);

    /* Release all resources */
    audio_pipeline_deinit(pipeline);
    audio_element_deinit(tone_stream_reader);
    audio_element_deinit(i2s_stream_writer);
    audio_element_deinit(wav_decoder);
}
