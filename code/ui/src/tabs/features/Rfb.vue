<template>
    <section>
        <div class="header">
            <h1>RADIO FREQUENCY</h1>
            <h2>
                Sonoff 433 RF Bridge &amp; RF Link Configuration<br>
            </h2>
        </div>

        <div>
            This page allows you to configure the RF codes
            for the Sonoff RFBridge 433 and also for a basic RF receiver.<br><br> To learn a new code click <strong>LEARN</strong>
            (the Sonoff RFBridge will beep) then press a button on the remote, the new code should show up (and the
            RFBridge will double beep). If the device double beeps but the code does not update it has not been
            properly learnt. Keep trying.<br><br> Modify or create new codes manually and then click
            <strong>SAVE</strong> to store them in the device memory. If your controlled device uses the same code
            to switch ON and OFF, learn the code with the ON button and copy paste it to the OFF input box, then
            click SAVE on the last one to store the value.<br><br> Delete any code clicking the
            <strong>FORGET</strong> button. <br><br>You can also specify any RAW code. For reference see <a
                rel="noopener" target="_blank" href="https://github.com/Portisch/RF-Bridge-EFM8BB1/wiki/Commands">possible
                commands for Sonoff RF Bridge EFM8BB1</a> (original firmware supports codes from <strong>0xA0</strong>
            to <strong>0xA5</strong>).
        </div>

        <div class="page">
            <fieldset>
                <legend>RF Codes</legend>

                <div id="rfbNodes"></div>

                <legend>Settings</legend>
                <div class="pure-g">
                    <label class="pure-u-1 pure-u-lg-1-4">Repeats</label>
                    <div class="pure-u-1 pure-u-lg-1-4">
                        <Inpt class="pure-u-1 pure-u-lg-23-24"
                              name="rfbRepeat"
                              type="number"
                              min="1"
                              tabindex="0"/>
                    </div>
                    <div class="pure-u-0 pure-u-lg-1-2"></div>
                    <div class="pure-u-0 pure-u-lg-1-4"></div>
                    <Hint>
                        Number of times to repeat transmission
                    </Hint>
                </div>

                <legend>GPIO</legend>
                <div class="pure-g module module-rfbdirect">
                    <Hint>
                        Pins used by the receiver (RX) and transmitter
                        (TX). Set to <strong>NONE</strong> to disable
                    </Hint>

                    <label class="pure-u-1 pure-u-lg-1-4">RX Pin</label>
                    <select class="pure-u-1 pure-u-lg-1-4 gpio-select" name="rfbRX"></select>
                    <div class="pure-u-0 pure-u-lg-1-2"></div>

                    <label class="pure-u-1 pure-u-lg-1-4">TX Pin</label>
                    <select class="pure-u-1 pure-u-lg-1-4 gpio-select" name="rfbTX"></select>
                    <div class="pure-u-0 pure-u-lg-1-2"></div>
                </div>
            </fieldset>
        </div>

        <div id="rfbNodeTemplate" class="template">
            <legend>Switch #<span></span></legend>

            <div class="pure-g">
                <div class="pure-u-1 pure-u-lg-1-4"><label>Switch ON</label></div>
                <Inpt class="pure-u-1 pure-u-lg-1-3"
                      type="text"
                      maxlength="116"
                      name="rfbcode"
                      data-id="1"
                      data-status="1"/>
                <div class="pure-u-1-3 pure-u-lg-1-8">
                    <button type="button" class="pure-u-23-24 btn btn-rfb-learn">LEARN</button>
                </div>
                <div class="pure-u-1-3 pure-u-lg-1-8">
                    <button type="button" class="pure-u-23-24 btn btn-rfb-send">SAVE</button>
                </div>
                <div class="pure-u-1-3 pure-u-lg-1-8">
                    <button type="button" class="pure-u-23-24 btn btn-rfb-forget">FORGET</button>
                </div>
            </div>

            <div class="pure-g">
                <div class="pure-u-1 pure-u-lg-1-4"><label>Switch OFF</label></div>
                <Inpt class="pure-u-1 pure-u-lg-1-3"
                      type="text"
                      maxlength="116"
                      name="rfbcode"
                      data-id="1"
                      data-status="0"/>
                <div class="pure-u-1-3 pure-u-lg-1-8">
                    <Btn name="rfb-learn" class="pure-u-23-24">LEARN</Btn>
                </div>
                <div class="pure-u-1-3 pure-u-lg-1-8">
                    <Btn name="rfb-send" class="pure-u-23-24">SAVE</Btn>
                </div>
                <div class="pure-u-1-3 pure-u-lg-1-8">
                    <button type="button" class="pure-u-23-24 btn btn-rfb-forget">FORGET</button>
                </div>
            </div>
        </div>
    </section>
</template>

<script>
    import Inpt from './../../components/Input';
    import Btn from "../../components/Button";
    import Hint from "../../components/Hint";

    export default {
        components: {
            Hint,
            Btn,
            Inpt
        },
        inheritAttrs: false,
    }
</script>

<style>
    /* -----------------------------------------------------------------------------
        RF Bridge panel
       -------------------------------------------------------------------------- */

    #panel-rfb fieldset {
        margin: 10px 2px;
        padding: 20px;
    }

    #panel-rfb input {
        margin-right: 5px;
    }

    #panel-rfb label {
        padding-top: 5px;
    }

    #panel-rfb input {
        text-align: center;
    }

    /* -----------------------------------------------------------------------------
        Table
       -------------------------------------------------------------------------- */

    .right {
        text-align: right;
    }

    table.dataTable.display tbody td {
        text-align: center;
    }

    .filtered {
        color: rgb(202, 60, 60);
    }

</style>