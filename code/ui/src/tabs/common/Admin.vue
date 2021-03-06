<template>
    <section class="admin">
        <div class="header">
            <h1>ADMINISTRATION</h1>
            <h2>Device administration and security settings</h2>
        </div>
        <Group v-model="device" class="page form" #default>
            <fieldset>
                <Row>
                    <C>
                        <label>Settings</label>
                    </C>
                    <C stretch>
                        <Btn name="settings-backup" tag="a" :download="downloadName" href="#" @click="doBackup">
                            Backup
                        </Btn>
                        <Btn name="settings-restore" color="primary" @click="() => $refs.restoreFile.open()">
                            Restore
                        </Btn>
                        <Inpt ref="restoreFile" name="restore" type="file"
                              accept=".json, text/json"
                              @change="restoreFile"/>
                        <Btn name="settings-factory" color="danger">Factory Reset</Btn>
                    </C>
                </Row>
                <Row>
                    <C><label>Admin password</label></C>
                    <C>
                        <Row>
                            <Inpt name="adminPass1" placeholder="New password"
                                  minlength="8"
                                  maxlength="63" type="password" action="reboot"
                                  autocomplete="false"
                                  spellcheck="false"/>
                        </Row>
                        <Row>
                            <Inpt name="adminPass2" placeholder="Repeat password"
                                  minlength="8"
                                  maxlength="63" type="password" action="reboot"
                                  autocomplete="false"
                                  spellcheck="false"/>
                        </Row>
                        <Hint>
                            The administrator password is used to access this web interface (user 'admin'), but also to
                            connect to the device when in AP mode or to flash a new firmware over-the-air (OTA).<br> It
                            must be <strong>8..63 characters</strong> (numbers and letters and any of these special
                            characters: _,.;:~!?@#$%^&amp;*&lt;&gt;\|(){}[]) and have at least <strong>one
                            lowercase</strong> and <strong>one uppercase</strong> or <strong>one number</strong>.
                        </Hint>
                    </C>
                </Row>
                <Row>
                    <C><label>HTTP port</label></C>
                    <C>
                        <Inpt name="webPort" type="text" action="reboot"/>
                        <Hint>
                            This is the port for the web interface and API requests. If different than 80 (standard HTTP
                            port) you will have to add it explicitly to your requests: http://myip:myport/
                        </Hint>
                    </C>
                </Row>
                <Row>
                    <C><label>Enable WS Auth</label></C>
                    <C>
                        <Inpt type="switch" name="wsAuth"/>
                    </C>
                </Row>
                <Row ref="api">
                    <C><label>Enable HTTP API</label></C>
                    <C>
                        <Inpt type="switch" name="apiEnabled"/>
                    </C>
                </Row>
                <Row ref="api">
                    <C><label>Restful API</label></C>
                    <C>
                        <Inpt type="switch" name="apiRestFul"/>
                        <Hint>
                            If enabled, API requests to change a status (like a relay) must be done using PUT. If
                            disabled you can issue them as GET requests (easier from a browser).
                        </Hint>
                    </C>
                </Row>
                <Row ref="api">
                    <C><label>Real time API</label></C>
                    <C>
                        <Inpt type="switch" name="apiRealTime"/>
                        <Hint>
                            By default, some magnitudes are being preprocessed and filtered to avoid spurious values. If
                            you want to get real-time values (not preprocessed) in the API turn on this setting.
                        </Hint>
                    </C>
                </Row>
                <Row ref="api">
                    <C><label>HTTP API Key</label></C>
                    <C no-wrap>
                        <Inpt name="apiKey" type="text"/>
                        <Btn name="apikey" color="primary">Auto</Btn>
                        <Hint>
                            This is the key you will have to pass with every HTTP request to the API, either to get or
                            write values. All API calls must contain the <strong>apikey</strong> parameter with the
                            value above. To know what APIs are enabled do a call to <strong>/apis</strong>.
                        </Hint>
                    </C>
                </Row>
                <Row ref="telnet">
                    <C><label>Enable TELNET</label></C>
                    <C>
                        <Inpt type="switch" name="telnetSTA"/>
                        <Hint>
                            Turn ON to be able to telnet to your device while connected to your home router.<br>TELNET
                            is always enabled in AP mode.
                        </Hint>
                    </C>
                </Row>
                <Row ref="telnet">
                    <C><label>TELNET Password</label></C>
                    <C>
                        <Inpt type="switch" name="telnetAuth"/>
                        <Hint>Request password when starting telnet session</Hint>
                    </C>
                </Row>
                <Row>
                    <C><label>Heartbeat message</label></C>
                    <C>
                        <Inpt type="select" name="hbMode"
                              :options="[
                                  'Disabled',
                                  'On device startup',
                                  'Repeat after defined interval',
                                  'Repeat only device status'
                              ]"/>
                        <Hint>Define when heartbeat message will be sent.</Hint>
                    </C>
                </Row>
                <Row>
                    <C><label>Heartbeat interval</label></C>
                    <C>
                        <Inpt name="hbInterval" type="number"/>
                        <Hint>
                            This is the interval in <strong>seconds</strong> how often to send the heartbeat message.
                        </Hint>
                    </C>
                </Row>
                <Row>
                    <C><label>Upgrade</label></C>
                    <C>
                        <Row no-wrap>
                            <Inpt type="text" :value="upgradeFile.name"
                                  readonly @click="() => $refs.upgradeFile.$el.click()"/>
                            <Btn ref="browse" name="upgrade-browse" @click="() => $refs.upgradeFile.$el.click()">
                                Browse
                            </Btn>
                            <Btn name="upgrade" color="danger">Upgrade</Btn>
                        </Row>
                        <Hint>
                            The device has {{device.free_size}} bytes available for OTA updates. If your image is larger
                            than this consider doing a
                            <A href="https://github.com/xoseperez/espurna/wiki/TwoStepUpdates" strong>two-step
                                update</A>.
                        </Hint>
                        <Row>
                            <progress id="upgrade-progress"></progress>
                        </Row>
                        <Inpt ref="upgradeFile" name="upgrade" type="file"

                              @change="(file) => {upgradeFile = file}"/>
                    </C>
                </Row>
            </fieldset>

            <div class="header">
                <h1>NTP</h1>
                <h2>
                    Configure your NTP (Network Time Protocol) servers and local configuration to keep your device time
                    up to the second for your location.
                </h2>
            </div>
            <div class="page form">
                <fieldset>
                    <Row>
                        <C><label>Device Current Time</label></C>
                        <C>
                            <Inpt name="now" type="text" readonly/>
                        </C>
                    </Row>
                    <Row>
                        <C><label>NTP Server</label></C>
                        <C>
                            <Inpt name="ntpServer" type="text"/>
                        </C>
                    </Row>
                    <Row>
                        <C><label>Time Zone</label></C>
                        <C>
                            <Inpt type="select" name="ntpTZ"
                                  :options="ntpOffsets"/>
                        </C>
                    </Row>
                </fieldset>
            </div>
        </Group>
        <iframe v-if="downloader" v-show="false" :href="downloader"></iframe>
    </section>
</template>
<script>
    import Inpt from "./../../components/Input";
    import Btn from "./../../components/Button";
    import A from "./../../components/ExtLink";
    import Row from "../../layout/Row";
    import C from "../../layout/Col";
    import Hint from "../../components/Hint";
    import Group from "../../components/Group";
    import ws from "../../common/websocket";
    import {alertError} from "../../common/notification";

    export default {
        components: {Group, Hint, C, Row, Inpt, Btn, A},
        inheritAttrs: false,
        props: {
            getSettings: Function,
            device: Object,
        },
        data() {
            return {
                upgradeFile: {},
                downloader: null,
            };
        },
        computed: {
            downloadName() {
                return "esp_backup_" + this.device.hostname.replace(/[\W_]+/g, "_") + ".json";
            },
            ntpOffsets() {
                // TODO this needs a new notation
                const tz = [
                    -720, -660, -600, -570, -540,
                    -480, -420, -360, -300, -240,
                    -210, -180, -120, -60, 0,
                    60, 120, 180, 210, 240,
                    270, 300, 330, 345, 360,
                    390, 420, 480, 510, 525,
                    540, 570, 600, 630, 660,
                    720, 765, 780, 840
                ];
                let offsets = [];

                tz.forEach((v) => {
                    const text = "GMT" + (v >= 0 ? "+" : "-") + new Date((1440 + Math.abs(v)) * 60000).toISOString().substr(11, 5);
                    offsets.push({k: v, l: text});
                });

                return offsets;
            }
        },
        methods: {
            doBackup(event) {
                try {
                    const json = JSON.stringify(this.getSettings());
                    event.target.href = "data:text/json;charset=utf-8," + encodeURIComponent(json);
                } catch (e) {
                    this.downloader = ws.urls.config;
                }
            },
            restoreFile(file) {
                try {
                    const fr = new FileReader();

                    fr.onload = (e) => {
                        //TODO make a setting parser and load the data in the UI directly
                        ws.send({action: "restore", data: JSON.parse(e.target.result)});
                    };

                    fr.readAsText(file);
                } catch (e) {
                    alertError("Your browser doesn't support the file reader API");
                }
            }
        }
    };
</script>
<style>
    #upgrade-progress {
        display: none;
        height: 20px;
        margin-top: 10px;
        width: 100%;
    }

    #uploader,
    #downloader {
        display: none;
    }

</style>
