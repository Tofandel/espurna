<template>
    <section v-loading="!isLoaded" class="wrapper">
        <i v-if="close" class="back" @click="close">🡠</i>
        <!-- TODO if process.env.VUE_APP_FORCE_PASS_CHANGE -->
        <Setup v-if="!webmode"/>
        <Form v-else ref="formSettings" v-model="data">
            <Menu id="layout" :tabs="tabs" class="webmode">
                <template #header>
                    <div class="heading">
                        <Icon class="icon"/>
                        <h1 class="hostname">{{data.device.hostname}}</h1>
                        <h2 class="desc">{{data.device.desc}}</h2>
                        <div class="clearfix"></div>
                    </div>
                </template>

                <template #footer>
                    <div class="main-buttons">
                        <Btn v-loading="saving" name="update" :disabled="Object.keys(modifiedSettings).length < 1"
                             @click="save">
                            Save
                        </Btn>
                        <Btn name="reconnect" color="primary" @click="reconnect">Reconnect</Btn>
                        <Btn name="reboot" color="danger" @click="reboot">Reboot</Btn>
                    </div>

                    <div class="footer">
                        &copy; 2016-2020<br><A href="https://twitter.com/xoseperez">Xose Pérez</A><br>
                        <A href="http://tinkerman.cat">http://tinkerman.cat</A><br>
                        <A href="https://github.com/xoseperez/espurna">ESPurna @ GitHub</A> <br>UI by
                        <A href="https://github.com/tofandel">Tofandel</A> <br>GPLv3 license<br><br> Version:
                        {{data.version.app_version}}
                    </div>
                </template>

                <template #status>
                    <Status v-bind="data" :last-update="lastUpdate"/>
                </template>

                <template #general>
                    <General v-bind="data"/>
                </template>

                <template #admin>
                    <Admin v-bind="data" :get-settings="() => flatten(data, received)"/>
                </template>

                <template v-if="data.modules.mqtt" #mqtt>
                    <Mqtt v-bind="data"/>
                </template>

                <!-- #!if THERMOSTAT -->
                <template v-if="data.modules.thermostat" #thermostat>
                    <Tstat v-bind="data"/>
                </template>
                <!-- #!endif -->

                <!-- #!if LED || BUTTON -->
                <template v-if="data.modules.led" #ledButton>
                    <LedButton v-bind="data" :relay-options="relayOptions"/>
                </template>
                <!-- #!endif -->

                <!-- #!if LIGHT -->
                <template v-if="data.modules.color" #color>
                    <Color v-bind="data"/>
                </template>
                <!-- #!endif -->

                <!-- #!if RFM69 -->
                <template v-if="data.modules.rfm69" #rfm69>
                    <Rfm69 v-bind="data"/>
                </template>
                <!-- #!endif -->

                <!-- #!if RFBRIDGE -->
                <template v-if="data.modules.rfb" #rfb>
                    <Rfb v-bind="data"/>
                </template>
                <!-- #!endif -->

                <!-- #!if SENSOR -->
                <template v-if="data.modules.sns" #sns>
                    <Sns v-bind="data"/>
                </template>
                <!-- #!endif -->

                <!-- #!if RELAYS -->
                <template v-if="data.modules.relay" #relays>
                    <Relays v-bind="data" :relay-options="relayOptions"/>
                </template>
                <!-- #!endif -->

                <!-- #!if LIGHTFOX -->
                <template v-if="data.modules.lightfox" #lightfox>
                    <Lfox v-bind="data"/>
                </template>
                <!-- #!endif -->

                <!-- #!if DCZ -->
                <template v-if="data.modules.dcz" #dcz>
                    <Dcz v-bind="data"/>
                </template>
                <!-- #!endif -->

                <!-- #!if HA -->
                <template v-if="data.modules.ha" #ha>
                    <Ha v-bind="data"/>
                </template>
                <!-- #!endif -->

                <!-- #!if ALEXA -->
                <template v-if="data.modules.alexa" #alexa>
                    <Alexa v-bind="data"/>
                </template>
                <!-- #!endif -->

                <!-- #!if THINGSPEAK -->
                <template v-if="data.modules.tspk" #thingspeak>
                    <Tspk v-bind="data"/>
                </template>
                <!-- #!endif -->

                <!-- #!if IDB -->
                <template v-if="data.modules.idb" #idb>
                    <Idb v-bind="data"/>
                </template>
                <!-- #!endif -->

                <!-- #!if NOFUSS -->
                <template v-if="data.modules.nofuss" #nofuss>
                    <Nfss v-bind="data"/>
                </template>
                <!-- #!endif -->
            </Menu>
        </Form>
    </section>
</template>

<script>

    import "./directives/loading";

    import ws from "./common/websocket";
    import Icon from "../public/icons/icon.svg";

    import Setup from "./tabs/common/Setup";
    import Inpt from "./components/Input";
    import Menu from "./components/Menu";
    import Form from "./components/Form";
    import A from "./components/ExtLink";
    import Btn from "./components/Button";


    import Admin from "./tabs/common/Admin";
    import General from "./tabs/common/General";
    import Status from "./tabs/common/Status";

    import diff from "deep-object-diff/dist/diff";
    import {confirm, alertError, alertWarning} from "./common/notification";
    import flatten from "./common/flatten";
    import prepareData from "./common/prepareData";

    let tabs = [
        //Basic settings
        {k: "status", l: "Status"}, //Move debug to status
        {k: "general", l: "General"}, //Move wifi to general
        {k: "admin", l: "Admin"}, //Move ntp to admin
        {k: "separator"}
    ];
    let components = {A, Icon, Inpt, Menu, Form, Admin, General, Status, Btn, Setup};

    //Board Features

    // #!if MQTT
    import Mqtt from "./tabs/features/Mqtt";

    components.Mqtt = Mqtt;
    tabs.push({k: "mqtt", l: "MQTT"});
    // #!endif

    // #!if THERMOSTAT
    import Tstat from "./tabs/features/Thermostat";

    components.Tstat = Tstat;
    tabs.push({k: "thermostat", l: "Thermostat"});
    // #!endif


    // #!if LED || BUTTON
    let label = [];
    // #!endif

    // #!if LED
    label.push("Led");
    // #!endif

    // #!if BUTTON
    label.push("Button");
    // #!endif

    // #!if LED || BUTTON
    import LedButton from "./tabs/features/LedButton";

    components.LedButton = LedButton;
    tabs.push({k: "ledButton", l: label.join("/")});
    // #!endif

    // #!if LIGHT
    import Color from "./tabs/features/Color";

    components.Color = Color;
    tabs.push({k: "color", l: "Lights"}); //Moved color schedules here
    // #!endif

    // #!if RFM69
    import Rfm69 from "./tabs/features/Rfm69";

    components.Rfm69 = Rfm69;
    tabs.push({k: "rfm69", l: "RFM69 Mapping"}); //Moved messages and mapping here
    // #!endif

    // #!if RFBRIDGE
    import Rfb from "./tabs/features/Rfb";

    components.Rfb = Rfb;
    tabs.push({k: "rfb", l: "RF Bridge"});
    // #!endif

    // #!if SENSOR
    import Sns from "./tabs/features/Sensors";

    components.Sns = Sns;
    tabs.push({k: "sns", l: "Sensors"});
    // #!endif

    // #!if RELAYS
    import Relays from "./tabs/features/Relays";

    components.Relays = Relays;
    tabs.push({k: "relays", l: "Switches"}); //Moved schedules to switches
    // #!endif

    tabs.push({k: "separator"});

    //Integrations
    // #!if HA
    import Ha from "./tabs/integrations/HomeAssistant";

    components.Ha = Ha;
    tabs.push({k: "ha", l: "Home Assistant"});
    // #!endif

    // #!if ALEXA
    import Alexa from "./tabs/integrations/Alexa";

    components.Alexa = Alexa;
    tabs.push({k: "alexa", l: "Alexa"});
    // #!endif

    // #!if LIGHTFOX
    import Lfox from "./tabs/features/LightFox";

    components.Lfox = Lfox;
    tabs.push({k: "lightfox", l: "LightFox"});
    // #!endif

    // #!if DCZ
    import Dcz from "./tabs/integrations/Domoticz";

    components.Dcz = Dcz;
    tabs.push({k: "dcz", l: "Domoticz"});
    // #!endif

    // #!if NOFUSS
    import Nfss from "./tabs/integrations/NoFuss";

    components.Nfss = Nfss;
    tabs.push({k: "nofuss", l: "NoFuss"});
    // #!endif

    // #!if THINGSPEAK
    import Tspk from "./tabs/integrations/ThingSpeak";

    components.Tspk = Tspk;
    tabs.push({k: "thingspeak", l: "ThingSpeak"});
    // #!endif

    // #!if IDB
    import Idb from "./tabs/integrations/InfluxDB";

    components.Idb = Idb;
    tabs.push({k: "idb", l: "InfluxDB"});
    // #!endif

    let messages = ["",
        "Remote update started",
        "OTA update started",
        "Error parsing data!",
        "The file does not look like a valid configuration backup or is corrupted",
        "Changes saved. You should reboot your board now",
        "Passwords do not match!",
        "Changes saved",
        "No changes detected",
        "Session expired, please reload page..."
    ];


    export default {
        components,
        props: {
            address: {
                type: String,
                required: false
            },
            close: {
                type: Function,
                required: false
            }
        },
        data() {
            return {
                lastUpdate: Date.now(),
                saving: false,
                webmode: true,
                data: {},
                received: {
                    _loaded: false,
                    _modules: {},
                    _version: {
                        app_version: process.env.VUE_APP_VERSION
                    },
                    device: {
                        _path: "",
                        hostname: "ESPURNA",
                        desc: "",
                        _now: Math.floor(Date.now() / 1000),
                        _uptime: 0,
                    },
                },
                tabs: tabs,
                interval: null
            };
        },
        computed: {
            isLoaded() {
                return this.original.loaded;
            },
            modified() {
                return diff(this.original, this.data);
            },
            modifiedSettings() {
                return this.flatten(this.modified, this.received);
            },
            original() {
                return JSON.parse(JSON.stringify(this.received).replace(/"_/g, "\""));
            },
            lightOptions() {
                let options = [];
                if ("light" in this.data) {
                    for (let i = 0; i < this.data.light.num_channel; ++i) {
                        options.push({k: i, l: "Channel #" + i});
                    }
                }
                return options;
            },
            relayOptions() {
                let options = [];
                if (this.data.relay && this.data.relay.list) {
                    this.data.relay.list.forEach((v, i) => {
                        options.push({k: i, l: "Switch " + (v.name || "#" + i) + " (" + v.gpio + ")"});
                    });
                }
                return options;
            },
        },
        watch: {
            original: {
                handler(rec, old) {
                    if (typeof old === "undefined" || old.loaded !== true
                        || Object.keys(this.modifiedSettings).length === 0) {
                        this.data = JSON.parse(JSON.stringify(this.original)); //Cheap deep clone
                    }
                    /*else {
                    // This is not yet needed but might be used to have synchronized changes between two instances or more of the interface
                        const dif = diff(this.modified, diff(old, rec));
                        console.log(dif, this.modified, diff(old, rec));
                    }*/
                },
                immediate: true,
                deep: true
            }
        },
        mounted() {
            if (!this.address) {
                // Check host param in query string
                const search = new URLSearchParams(window.location.search),
                    host = search.get("host");
                ws.connect(host ? host : window.location.host, this.receiveMessage);
            } else {
                ws.connect(this.address, this.receiveMessage);
            }
        },
        beforeDestroy() {
            clearInterval(this.interval);
            ws.close();
        },
        methods: {
            flatten: flatten,
            save() {
                if (this.$refs.formSettings.reportValidity()) {
                    if (Object.keys(this.modifiedSettings).length) {
                        this.saving = true;
                       ws.send({config: this.modifiedSettings}, () => {
                            prepareData(this.received, this.modified);
                            this.saving = false;
                        }, true);
                    } else {
                        alertWarning({title: "Nothing to save", message: "Data has not been modified"});
                    }
                } else {
                    alertError({
                        title: "The data entered is not valid"
                    });
                    //TODO focus invalid
                }
            },
            reconnect(ask) {
                let question = ask ? null : "Are you sure you want to disconnect from the current WIFI network?";
                this.doAction(question, "reconnect").then(() => {
                    this.doReload(1000);
                });
            },
            reboot(ask) {
                let question = ask ? null : "Are you sure you want to reboot the device?";
                this.doAction(question, "reboot").then(() => {
                    this.doReload(4000);
                });
            },
            askSave() {
                return new Promise((resolve) => {
                    if (this.modified) {
                        confirm("Some changes have not been saved yet, do you want to save them first?").then(() => {
                            ws.send({config: this.modified}, () => {
                                resolve();
                            }, true);
                        }).catch(resolve);
                    } else {
                        resolve();
                    }
                });
            },
            askQuestion(question) {
                return new Promise((resolve) => {
                    if (question) {
                        confirm(question).then(resolve);
                    } else {
                        resolve();
                    }
                });
            },
            doAction(question, action, data) {
                return new Promise((resolve, fail) => {
                    this.askSave().then(() => {
                        this.askQuestion(question).then(() => {
                            ws.send({action, data}, () => {
                                resolve();
                            });
                        }).catch(fail);
                    }).catch(fail);
                });
            },
            doReload(wait_time) {
                ws.retry(wait_time);
                ws.close(1000, "Device reload");
            },
            receiveMessage(data) {
                if (data.action === "reload") {
                    this.doReload();
                } else {
                    prepareData(this.received, data);
                    this.lastUpdate = Date.now();
                }
            }
        },
        provide() {
            return {
                $app: {
                    ws: this.ws,
                    node: this
                }
            };
        }
    };

</script>

<style lang="less">
    @import "assets/Colors";
    @import "assets/Loaders";

    /* -----------------------------------------------------------------------------
    General
   -------------------------------------------------------------------------- */
    .icon {
        width: 30px;
        margin: 7px 10px 7px 0;
        height: auto;
        float: left;

        path {
            fill: #fff;
        }
    }

    .clearfix {
        clear: both;
    }

    @media screen and (max-width: 32em) {
        .header > h1 {
            line-height: 100%;
            font-size: 2em;
        }
    }

    .align-right {
        text-align: right;

        &.pd {
            padding-right: 5px;
        }
    }

    .align-left {
        text-align: left;

        &.pd {
            padding-left: 5px;
        }
    }

    .center {
        text-align: center;

        &.pd {
            padding-right: 5px;
            padding-left: 5px;
        }
    }

    .hostname {
        font-size: 1em;
    }

    .desc {
        font-size: .8em;
        font-weight: normal;
    }

    .page {
        margin-top: 10px;
    }

    .hint {
        color: #ccc;
        font-size: 80%;
        margin: -10px 0 10px 0;
        display: none;
    }

    .hint a {
        color: inherit;
    }

    .template {
        display: none;
    }

    input.center {
        margin-bottom: 0;
    }

    div.center {
        margin: .5em 0 1em;
    }

    .align-right {
        text-align: right;
    }

    textarea.terminal, div.terminal, pre.terminal {
        font-family: 'Courier New', monospace;
        font-size: 80%;
        background-color: #333;
        color: #52ff5f;
        line-height: 1.2em;
    }


    /* -----------------------------------------------------------------------------
        Sliders
       -------------------------------------------------------------------------- */

    input.slider {
        margin-top: 10px;
    }

    span.slider {
        font-size: 70%;
        letter-spacing: 0;
        margin-left: 10px;
        margin-top: 7px;
    }


    /* -----------------------------------------------------------------------------
        Menu
       -------------------------------------------------------------------------- */


    // Side menu.css
    body {
        color: #777;
    }

    /*
    The content `<div>` is where all your content goes.
    */
    .header {
        color: #333;
        border-bottom: 1px solid #eee;
        width: 100%;
        padding: 2em 0 1em;
        z-index: 79;
        text-align: center;
    }

    .page + .header {
        margin-top: 40px;
        border-top: 1px solid #eee;
    }

    .header h1 {
        margin: 0.2em 0;
        font-size: 2.6em;
        font-weight: 300;
    }

    .header h2 {
        font-weight: 300;
        color: #ccc;
        padding: 0;
        margin-top: 0;
    }

    .content-subhead {
        margin: 50px 0 20px 0;
        font-weight: 300;
        color: #888;
    }

    html {
        font-family: sans-serif;
        -webkit-text-size-adjust: 100%
    }

    body, html, body, body > form {
        width: 100%;
        height: 100%;
    }

    body * {
        box-sizing: border-box;
    }

    body {
        margin: 0
    }

    b, strong {
        font-weight: 700
    }

    h1 {
        font-size: 2em;
        margin: .67em 0
    }

    img {
        border: 0
    }

    hr {
        box-sizing: content-box;
        height: 0
    }

    pre {
        overflow: auto
    }

    code, pre {
        font-family: monospace;
        font-size: 1em
    }

    button, input, select, textarea {
        color: inherit;
        font: inherit;
    }

    input {
        line-height: normal
    }

    input[type=checkbox], input[type=radio] {
        padding: 0
    }

    fieldset {
        border: 1px solid silver;
        margin: 0 2px;
        padding: .35em .625em .75em
    }

    legend {
        border: 0;
        padding: 0
    }

    textarea {
        overflow: auto;
        width: 100% !important;
        height: 200px;
    }

    table {
        border-collapse: collapse;
        border-spacing: 0
    }

    td, th {
        padding: 0
    }

    .hidden, [hidden] {
        display: none !important
    }

    i.back {
        cursor: pointer;
        font-style: normal;
        font-size: 1.5em;
        position: absolute;
        display: block;
        color: white;
        top: 0;
        right: 0;
        width: 40px;
        height: 40px;
        text-align: center;
        line-height: 38px;
        border-color: darken(@primary, 15%);
        border-style: solid;
        border-width: 0 0 3px 0;
        border-radius: 0 0 0 5px;
        background: @primary;
        z-index: 100;
    }

    .wrapper {
        max-width: 100vw;
        overflow-x: hidden;
    }

    @media screen and (max-width: 48em) {
        .header {
            text-align: center;
            padding: 2.5em 2em 0;
        }
    }
</style>
