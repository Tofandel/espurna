<template>
    <Form id="setup" v-model="settings" autocomplete="off">
        <div class="panel block">
            <div class="header">
                <h1>SETUP</h1>
                <h2>
                    Before using this device you have to change the default password for the user
                    <strong>admin</strong>.
                    This password will be used for the <strong>AP mode hotspot</strong>, the
                    <strong>web interface</strong> (where you are now) and the
                    <strong>over-the-air updates</strong>.
                </h2>
            </div>

            <div class="page">
                <fieldset>
                    <Row>
                        <C class="align-right pd"><label for="adminPass1">New Password</label></C>
                        <C class="align-left pd">
                            <Inpt id="adminPass1"
                                  ref="adminPass1"
                                  name="adminPass1"
                                  minlength="8"
                                  maxlength="63"
                                  type="password"
                                  tabindex="1"
                                  autocomplete="false"
                                  spellcheck="false"
                                  :pattern="passRegex"
                                  required/>
                            <Hint>
                                Password must be <strong>8..63 characters</strong> (numbers and letters and any of
                                these special characters:
                                <pre>-_,.;:~!?@#$%^&amp;*&lt;&gt;\/|=(){}[]</pre>
                                ) and have at least
                                <strong>one lowercase</strong> and <strong>one uppercase</strong> or
                                <strong>one number</strong>.
                            </Hint>
                        </C>
                    </Row>

                    <Row>
                        <C class="align-right pd"><label for="adminPass2">Repeat password</label></C>
                        <C class="align-left pd">
                            <Inpt id="adminPass2"
                                  ref="adminPass2"
                                  name="adminPass2"
                                  minlength="8"
                                  maxlength="63"
                                  type="password"
                                  tabindex="2"
                                  autocomplete="false"
                                  spellcheck="false"
                                  required
                                  @input="checkPasswords"/>
                        </C>
                    </Row>
                    <Row>
                        <C class="align-right">
                            <Btn name="generate-pwd" color="secondary"
                                 title="Generate password based on password policy" @click="generatePassword">
                                Generate
                            </Btn>
                        </C>
                    </Row>
                </fieldset>
                <fieldset>
                    <legend>Networks</legend>
                    <Wifi v-model="wifi.list" :max="wifi.max"/>

                    <Row>
                        <C class="center" stretch :size="4" :offset="3">
                            <Btn name="save"
                                 title="Save">
                                Save
                            </Btn>
                        </C>
                    </Row>
                </fieldset>
                <fieldset>
                    <legend>Or restore the settings from a backup</legend>
                    <input type="text" :value="restoreFile.name" readonly @click="() => $refs.restoreFile.$el.click()">
                    <Btn name="settings-restore" color="primary" @click="() => $refs.restoreFile.el.click()">
                        Restore
                    </Btn>
                    <Inpt ref="restoreFile" name="restore" type="file" @change="(file) => { restoreFile = file }"/>
                </fieldset>
            </div>
        </div>
    </Form>
</template>

<script>
    import Btn from "../../components/Button";
    import Inpt from "../../components/Input";
    import Hint from "../../components/Hint";
    import C from "../../layout/Col";
    import Row from "../../layout/Row";
    import Form from "../../components/Form";
    import Wifi from "../../components/Wifi";

    export default {
        components: {Wifi, Form, Row, C, Hint, Btn, Inpt},
        inheritAttrs: false,
        props: {
            settings: {
                type: Object,
                default: () => ({})
            },
            wifi: {
                type: Object,
                default: () => ({})
            }
        },
        data() {
            return {
                passRegex: "(?=^.*[a-z]+.*$)^(?=^.*[A-Z0-9]+.*$)^[a-zA-Z0-9-_,\\.;:~!\\?@#\\$%\\^&\\*<>\\|=\\(\\)/{}\\[\\]]{8,63}$",
                restoreFile: {}
            };
        },
        methods: {
            checkPasswords() {
                if (this.$refs.adminPass1.val !== this.$refs.adminPass2.val) {
                    this.$refs.adminPass2.$refs.input.setCustomValidity("Passwords do not match.");
                }
            },
            generatePassword() {
                let pass;

                do {
                    pass = new Array(12).fill(0).map(() => String.fromCharCode(Math.random() * 86 + 40)).join("");
                } while (!pass.match(new RegExp(this.passRegex)));

                this.$refs.adminPass1.val = this.$refs.adminPass2.val = pass;
                this.$refs.adminPass1.setVisible();
            }
        }
    };
</script>

<style lang="less">
    #setup {
        max-width: 800px;
        height: auto;
        padding: 10px;
        position: absolute;
        top: 40%;
        left: 50%;
        transform: translate(-50%, -50%);
    }
</style>
