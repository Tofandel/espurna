<template>
    <section class="status">
        <div class="header">
            <h1>STATUS</h1>
            <h2>Current configuration</h2>
        </div>

        <div class="page">
            <fieldset>
                <Repeater v-if="relay.state" v-model="relay.state.list" class="relays" locked>
                    <template #default="tpl">
                        <Row>
                            <C :size="2"><label>Switch #{{tpl.k}}</label></C>
                            <C :size="8">
                                <Inpt type="switch" name="status"
                                      on="ON"
                                      off="OFF"
                                      :disabled="tpl.value.lock < 2"
                                      @input="() => {}"/>
                            </C>
                        </Row>
                    </template>
                </Repeater>

                <!-- #if process.env.VUE_APP_LIGHT === 'true' -->
                <template v-if="modules.light">
                    <Row>
                        <C><label>Color</label></C>
                        <C><span>{{light.color}}</span></C>
                    </Row>

                    <Row>
                        <C><label>Mireds (Cold &harr; Warm)</label></C>
                        <C>
                            <Inpt type="range" name="mireds" min="153" max="500" :value="light.mired"/>
                            {{light.mired}}
                        </C>
                    </Row>

                    <Row>
                        <C><label>Brightness</label></C>
                        <C>
                            <input type="range" min="0" max="255"
                                   :value="light.brightness"
                                   readonly>
                            {{light.brightness}}
                        </C>
                    </Row>

                    <Row v-for="(channel, id) in light.channels" :key="id">
                        <C><label>Channel #{{id}}</label></C>
                        <C>
                            <input type="range" min="0" max="255"
                                   :value="channel"
                                   readonly>
                        </C>
                    </Row>
                </template>
                <!-- #endif -->

                <!-- #if process.env.VUE_APP_SENSOR === 'true' -->
                <template v-if="modules.sns">
                    <Row v-for="(magnitude, i) in magnitudes.list" :key="magnitude.index">
                        <C><label>{{magnitudeType(magnitude.type)}} #{{magnitude.index}},</label></C>
                        <C>
                            <Inpt class="pure-u-1 pure-u-lg-23-24 center" type="text" name="magnitude"
                                  :value="i"/>
                            <Hint>{{magnitude.description}}</Hint>
                        </C>
                    </Row>
                </template>
                <!-- #endif -->

                <!-- #if process.env.VUE_APP_RFM69 === 'true' -->
                <template v-if="modules.rfm69">
                    <Row>
                        <C><label>Packet count</label></C>
                        <C>{{rfm69.packet_count}}</C>
                    </Row>

                    <Row>
                        <C><label>Node count</label></C>
                        <C>{{rfm69.node_count}}</C>
                    </Row>
                </template>
                <!-- #endif -->

                <Row class="state responsive">
                    <C>
                        <Row>
                            <C><label>Manufacturer</label></C>
                            <C>{{device.manufacturer}}</C>

                            <C><label>Device</label></C>
                            <C>{{device.name}}</C>

                            <C>Chip ID</C>
                            <C>{{device.chip_id}}</C>

                            <C>SDK version</C>
                            <C>{{version.sdk}}</C>

                            <C>Core version</C>
                            <C>{{version.core}}</C>

                            <C>Firmware name</C>
                            <C>{{version.app_name}}</C>

                            <C>Firmware version</C>
                            <C>{{version.app_version}}</C>

                            <template v-if="version.app_revision">
                                <C>Firmware revision</C>
                                <C>{{version.app_revision}}</C>
                            </template>

                            <C>Firmware build date</C>
                            <C>{{version.app_build}}</C>

                            <C>Firmware size</C>
                            <C>{{version.sketch_size}}</C>

                            <C>Free space</C>
                            <C>{{device.free_size}} bytes</C>

                            <C>Free heap</C>
                            <C>{{device.heap}} bytes</C>

                            <C>Load average</C>
                            <C>{{device.load_average}}%</C>

                            <C>VCC</C>
                            <C>{{device.vcc}} mV</C>
                        </Row>
                    </C>

                    <C>
                        <Row>
                            <C>Network</C>
                            <C>{{wifi.name}}</C>

                            <C>Wifi MAC</C>
                            <C>{{wifi.mac}}</C>

                            <C>BSSID</C>
                            <C>{{wifi.bssid}}</C>

                            <C>Channel</C>
                            <C>{{wifi.channel}}</C>

                            <C>RSSI</C>
                            <C>{{wifi.rssi}}</C>

                            <C>IP</C>
                            <C>
                                <a :href="'//'+wifi.ip">{{wifi.ip}}</a>
                                (<a :href="'telnet://'+wifi.ip">telnet</a>)
                            </C>

                            <template v-if="modules.mqtt">
                                <C>MQTT Status</C>
                                <C>{{mqtt.status ? 'CONNECTED' : 'NOT CONNECTED'}}</C>
                            </template>

                            <template v-if="modules.ntp">
                                <C>NTP Status</C>
                                <C>{{ntp.status ? 'SYNCED' : 'NOT SYNCED'}}</C>
                                <C>Current time</C>
                                <C>{{date(device.now)}}</C>
                            </template>

                            <C>Uptime</C>
                            <C>{{elapsed(device.uptime)}}</C>

                            <C>Last update</C>
                            <C>{{device.lastUpdate}} seconds ago</C>
                        </Row>
                    </C>
                </Row>
            </fieldset>
        </div>

        <div class="header">
            <h1>DEBUG LOG</h1>
            <h2>
                Shows debug messages from the device
            </h2>
        </div>

        <div class="page">
            <fieldset>
                <Row v-if="modules.cmd" class="responsive">
                    <C :size="2"><label>Command</label></C>
                    <C :size="8" no-wrap>
                        <Inpt name="dbgcmd" type="text" tabindex="2"/>
                        <Btn name="dbgcmd">
                            Send
                        </Btn>
                        <Hint>
                            Write a command and click send to execute it on the device. The output will be shown
                            in the debug text area below.
                        </Hint>
                    </C>
                </Row>

                <Row>
                    <textarea v-model="logs"
                              class="terminal"
                              wrap="soft"
                              readonly spellcheck="false">
                    </textarea>
                </Row>
                <Row>
                    <Btn name="dbg-clear" color="danger">
                        Clear
                    </Btn>
                </Row>
            </fieldset>
        </div>
    </section>
</template>

<script>
    import Inpt from './../../components/Input';
    import Btn from './../../components/Button'
    import Row from "../../layout/Row";
    import C from "../../layout/Col";
    import Hint from "../../components/Hint";
    import Repeater from "../../components/Repeater";

    export default {
        components: {
            Repeater,
            Hint,
            C,
            Row,
            Inpt,
            Btn
        },
        inheritAttrs: false,
        props: {
            version: {
                type: Object,
            },
            relay: {
                type: Object,
                default: () => ({})
            },
            modules: {
                type: Object,
                default: () => ({})
            },
            magnitudes: {
                type: Object,
                default: () => ({})
            },
            device: {
                type: Object,
            },
            wifi: {
                type: Object,
                default: () => ({})
            },
            light: {
                type: Object,
                default: () => ({channels: []})
            },
            rfm69: {
                type: Object,
                default: () => ({})
            },
            mqtt: {
                type: Object,
                default: () => ({})
            },
            ntp: {
                type: Object,
                default: () => ({})
            },
            weblog: {
                type: Object,
                default: () => ({})
            }
        },
        data() {
            return {}
        },
        computed: {
            logs() {
                let str = "";
                if (this.weblog.msg)
                    this.weblog.msg.forEach((v, i) => {
                        str += this.weblog.pre[i] + v + "\n";
                    });
                return str;
            }
        },
        methods: {
            date(d) {
                return new Date(d * 1000).toLocaleString(navigator.languages);
            },
            elapsed(d) {
                try {
                    return parseInt(d / 86400) + "d " + new Date(d * 1000).toISOString().replace(/.*(\d{2}):(\d{2}):(\d{2}).*/, "$1h $2m $3s");
                } catch (e) {
                    return ""
                }
            },
            toggleRelay(id, val) {
                sendAction("relay", {id: id, status: value ? 1 : 0});
                this.$set(relays, id, !val)
            },

            magnitudeType(type) {
                const types = [
                    "Temperature", "Humidity", "Pressure",
                    "Current", "Voltage", "Active Power", "Apparent Power",
                    "Reactive Power", "Power Factor", "Energy", "Energy (delta)",
                    "Analog", "Digital", "Event",
                    "PM1.0", "PM2.5", "PM10", "CO2", "Lux", "UVA", "UVB", "UV Index", "Distance", "HCHO",
                    "Local Dose Rate", "Local Dose Rate",
                    "Count", "NO2", "CO", "Resistance", "pH"
                ];
                if (1 <= type && type <= types.length) {
                    return types[type - 1];
                }
                return null;
            }
        }
    }
</script>

<style>
    .state {
        border-top: 1px solid #eee;
        margin-top: 20px;
        padding-top: 30px;
        font-size: 80%;
    }


    .state .row .col:nth-child(odd) {
        text-align: right;
    }

    .state .row .col:nth-child(even) {
        font-weight: bold;
        text-align: left;
    }

    .relays .col > label {
        font-size: 1.3em;
        float: right;
        margin: .4em 15px;
    }

    /* -----------------------------------------------------------------------------
        Logs
       -------------------------------------------------------------------------- */

    #weblog {
        height: 400px;
        margin-bottom: 10px;
    }
</style>