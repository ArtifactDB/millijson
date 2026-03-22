<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.12.0">
  <compound kind="file">
    <name>millijson.hpp</name>
    <path>millijson/</path>
    <filename>millijson_8hpp.html</filename>
    <class kind="class">millijson::Base</class>
    <class kind="class">millijson::Number</class>
    <class kind="class">millijson::NumberAsString</class>
    <class kind="class">millijson::String</class>
    <class kind="class">millijson::Boolean</class>
    <class kind="class">millijson::Nothing</class>
    <class kind="class">millijson::Array</class>
    <class kind="class">millijson::Object</class>
    <class kind="struct">millijson::ParseOptions</class>
    <class kind="struct">millijson::DefaultProvisioner</class>
    <namespace>millijson</namespace>
  </compound>
  <compound kind="class">
    <name>millijson::Array</name>
    <filename>classmillijson_1_1Array.html</filename>
    <base>millijson::Base</base>
    <member kind="function">
      <type></type>
      <name>Array</name>
      <anchorfile>classmillijson_1_1Array.html</anchorfile>
      <anchor>ae850614d0bed0b5a01019130d80d94a1</anchor>
      <arglist>(std::vector&lt; std::shared_ptr&lt; Base &gt; &gt; x)</arglist>
    </member>
    <member kind="function">
      <type>Type</type>
      <name>type</name>
      <anchorfile>classmillijson_1_1Array.html</anchorfile>
      <anchor>a2b7598fd4ed816ee711eab9a50e4a4ee</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>const std::vector&lt; std::shared_ptr&lt; Base &gt; &gt; &amp;</type>
      <name>value</name>
      <anchorfile>classmillijson_1_1Array.html</anchorfile>
      <anchor>ac69079a94ad56ea767fe836add058933</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::shared_ptr&lt; Base &gt; &gt; &amp;</type>
      <name>value</name>
      <anchorfile>classmillijson_1_1Array.html</anchorfile>
      <anchor>ab647f3a4340d33e2bbc4eb835425b65c</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>millijson::Base</name>
    <filename>classmillijson_1_1Base.html</filename>
    <member kind="function" virtualness="pure">
      <type>virtual Type</type>
      <name>type</name>
      <anchorfile>classmillijson_1_1Base.html</anchorfile>
      <anchor>a590630b66e2b0d00bf7f3e0ccfba7a53</anchor>
      <arglist>() const =0</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>millijson::Boolean</name>
    <filename>classmillijson_1_1Boolean.html</filename>
    <base>millijson::Base</base>
    <member kind="function">
      <type></type>
      <name>Boolean</name>
      <anchorfile>classmillijson_1_1Boolean.html</anchorfile>
      <anchor>a35035805b6fb4f0074d9bee994a894b8</anchor>
      <arglist>(bool x)</arglist>
    </member>
    <member kind="function">
      <type>Type</type>
      <name>type</name>
      <anchorfile>classmillijson_1_1Boolean.html</anchorfile>
      <anchor>a5b31c83c3bc2690bf30df465ca8e49fd</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>const bool &amp;</type>
      <name>value</name>
      <anchorfile>classmillijson_1_1Boolean.html</anchorfile>
      <anchor>addf5ae365d8555e40cee67e253ab29f7</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>bool &amp;</type>
      <name>value</name>
      <anchorfile>classmillijson_1_1Boolean.html</anchorfile>
      <anchor>af3d5ff979e2e7a3239f1ddf486fc80e0</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>millijson::DefaultProvisioner</name>
    <filename>structmillijson_1_1DefaultProvisioner.html</filename>
    <member kind="typedef">
      <type>::millijson::Base</type>
      <name>Base</name>
      <anchorfile>structmillijson_1_1DefaultProvisioner.html</anchorfile>
      <anchor>ac75e8d09b380a537fceb1df812f7d1eb</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Boolean *</type>
      <name>new_boolean</name>
      <anchorfile>structmillijson_1_1DefaultProvisioner.html</anchorfile>
      <anchor>ab6d05a2bc22d435567b053b44fae472f</anchor>
      <arglist>(bool x)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Number *</type>
      <name>new_number</name>
      <anchorfile>structmillijson_1_1DefaultProvisioner.html</anchorfile>
      <anchor>a49b1c538b56c2a86eea0bb9c50df6732</anchor>
      <arglist>(double x)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static NumberAsString *</type>
      <name>new_number_as_string</name>
      <anchorfile>structmillijson_1_1DefaultProvisioner.html</anchorfile>
      <anchor>a0332031f7b82e8b11e52530c03598008</anchor>
      <arglist>(std::string x)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static String *</type>
      <name>new_string</name>
      <anchorfile>structmillijson_1_1DefaultProvisioner.html</anchorfile>
      <anchor>ad809bd41aaa4584ad5f0dd4215ac7154</anchor>
      <arglist>(std::string x)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Nothing *</type>
      <name>new_nothing</name>
      <anchorfile>structmillijson_1_1DefaultProvisioner.html</anchorfile>
      <anchor>aa79a92f8adbf190fa54b01aa92338d4b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Array *</type>
      <name>new_array</name>
      <anchorfile>structmillijson_1_1DefaultProvisioner.html</anchorfile>
      <anchor>a022b02abcda66a7dbe0351082c5fa2ad</anchor>
      <arglist>(std::vector&lt; std::shared_ptr&lt; Base &gt; &gt; x)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Object *</type>
      <name>new_object</name>
      <anchorfile>structmillijson_1_1DefaultProvisioner.html</anchorfile>
      <anchor>a5d360fd81f7feb8ba67ea681c0a2c1e7</anchor>
      <arglist>(std::unordered_map&lt; std::string, std::shared_ptr&lt; Base &gt; &gt; x)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>millijson::Nothing</name>
    <filename>classmillijson_1_1Nothing.html</filename>
    <base>millijson::Base</base>
    <member kind="function">
      <type>Type</type>
      <name>type</name>
      <anchorfile>classmillijson_1_1Nothing.html</anchorfile>
      <anchor>a549d250b32c4cd6ed92d23524ce2392f</anchor>
      <arglist>() const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>millijson::Number</name>
    <filename>classmillijson_1_1Number.html</filename>
    <base>millijson::Base</base>
    <member kind="function">
      <type></type>
      <name>Number</name>
      <anchorfile>classmillijson_1_1Number.html</anchorfile>
      <anchor>a70c2370c0d8ae254d4357d0c9bbf6f06</anchor>
      <arglist>(double x)</arglist>
    </member>
    <member kind="function">
      <type>Type</type>
      <name>type</name>
      <anchorfile>classmillijson_1_1Number.html</anchorfile>
      <anchor>a258f53a36e8ec12be2ed0936415131f1</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>const double &amp;</type>
      <name>value</name>
      <anchorfile>classmillijson_1_1Number.html</anchorfile>
      <anchor>a2a507dcf890f13e2974508bf792d1ee6</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>double &amp;</type>
      <name>value</name>
      <anchorfile>classmillijson_1_1Number.html</anchorfile>
      <anchor>a0c2e4394d1fbbfb723a69d76e5981619</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>millijson::NumberAsString</name>
    <filename>classmillijson_1_1NumberAsString.html</filename>
    <base>millijson::Base</base>
    <member kind="function">
      <type></type>
      <name>NumberAsString</name>
      <anchorfile>classmillijson_1_1NumberAsString.html</anchorfile>
      <anchor>a6572b283f7adc05f8f34ad7b7c0faca6</anchor>
      <arglist>(std::string x)</arglist>
    </member>
    <member kind="function">
      <type>Type</type>
      <name>type</name>
      <anchorfile>classmillijson_1_1NumberAsString.html</anchorfile>
      <anchor>a46c43bddb0fee5e2c5f7f235e2043534</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>const std::string &amp;</type>
      <name>value</name>
      <anchorfile>classmillijson_1_1NumberAsString.html</anchorfile>
      <anchor>a217b1a3e73db1de8c3427be65c7a037c</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>std::string &amp;</type>
      <name>value</name>
      <anchorfile>classmillijson_1_1NumberAsString.html</anchorfile>
      <anchor>a6fb54c068e7eaab0b24b2f8cba637aa6</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>millijson::Object</name>
    <filename>classmillijson_1_1Object.html</filename>
    <base>millijson::Base</base>
    <member kind="function">
      <type></type>
      <name>Object</name>
      <anchorfile>classmillijson_1_1Object.html</anchorfile>
      <anchor>ac5a7db89c4016a56933af21938968675</anchor>
      <arglist>(std::unordered_map&lt; std::string, std::shared_ptr&lt; Base &gt; &gt; x)</arglist>
    </member>
    <member kind="function">
      <type>Type</type>
      <name>type</name>
      <anchorfile>classmillijson_1_1Object.html</anchorfile>
      <anchor>acfe106773ae2807a0a362b5146617c02</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>const std::unordered_map&lt; std::string, std::shared_ptr&lt; Base &gt; &gt; &amp;</type>
      <name>value</name>
      <anchorfile>classmillijson_1_1Object.html</anchorfile>
      <anchor>a0e1c3767e475313bafed81fe87699ab3</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>std::unordered_map&lt; std::string, std::shared_ptr&lt; Base &gt; &gt; &amp;</type>
      <name>value</name>
      <anchorfile>classmillijson_1_1Object.html</anchorfile>
      <anchor>a104beefb4be2b5024e4d07f0e8029f0f</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>millijson::ParseOptions</name>
    <filename>structmillijson_1_1ParseOptions.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>number_as_string</name>
      <anchorfile>structmillijson_1_1ParseOptions.html</anchorfile>
      <anchor>a071217fba269c8088c8095a364261e73</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::size_t</type>
      <name>buffer_size</name>
      <anchorfile>structmillijson_1_1ParseOptions.html</anchorfile>
      <anchor>ae9d16685b58f11e86aecaaf094021522</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>parallel</name>
      <anchorfile>structmillijson_1_1ParseOptions.html</anchorfile>
      <anchor>a48e77696804d3227690e484b89a30662</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>millijson::String</name>
    <filename>classmillijson_1_1String.html</filename>
    <base>millijson::Base</base>
    <member kind="function">
      <type></type>
      <name>String</name>
      <anchorfile>classmillijson_1_1String.html</anchorfile>
      <anchor>ad09243f81b2f696e507770d80e769b74</anchor>
      <arglist>(std::string x)</arglist>
    </member>
    <member kind="function">
      <type>Type</type>
      <name>type</name>
      <anchorfile>classmillijson_1_1String.html</anchorfile>
      <anchor>a1d4eb4f12c092177662129161b543f51</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>const std::string &amp;</type>
      <name>value</name>
      <anchorfile>classmillijson_1_1String.html</anchorfile>
      <anchor>a0f526b71156470fe6e245d36faab2b62</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>std::string &amp;</type>
      <name>value</name>
      <anchorfile>classmillijson_1_1String.html</anchorfile>
      <anchor>a1aaacf2106a16c70fca09eb21dacd11d</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>millijson</name>
    <filename>namespacemillijson.html</filename>
    <class kind="class">millijson::Array</class>
    <class kind="class">millijson::Base</class>
    <class kind="class">millijson::Boolean</class>
    <class kind="struct">millijson::DefaultProvisioner</class>
    <class kind="class">millijson::Nothing</class>
    <class kind="class">millijson::Number</class>
    <class kind="class">millijson::NumberAsString</class>
    <class kind="class">millijson::Object</class>
    <class kind="struct">millijson::ParseOptions</class>
    <class kind="class">millijson::String</class>
    <member kind="enumeration">
      <type></type>
      <name>Type</name>
      <anchorfile>namespacemillijson.html</anchorfile>
      <anchor>aa16302fdbcf6f85a442a302b1745afb5</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; typename DefaultProvisioner::Base &gt;</type>
      <name>parse</name>
      <anchorfile>namespacemillijson.html</anchorfile>
      <anchor>a222e73844fb11f9d000e18e25a2cd940</anchor>
      <arglist>(Input_ &amp;input, const ParseOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Type</type>
      <name>validate</name>
      <anchorfile>namespacemillijson.html</anchorfile>
      <anchor>a3a3e891c2181292c17ec4196b6059b5a</anchor>
      <arglist>(Input_ &amp;input, const ParseOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; typename Provisioner_::Base &gt;</type>
      <name>parse_string</name>
      <anchorfile>namespacemillijson.html</anchorfile>
      <anchor>a41e7981d02afa5937e2db706f68444ec</anchor>
      <arglist>(const char *ptr, std::size_t len, const ParseOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Type</type>
      <name>validate_string</name>
      <anchorfile>namespacemillijson.html</anchorfile>
      <anchor>a13b89e03e4e6b5f12cde0e3c028224f3</anchor>
      <arglist>(const char *ptr, std::size_t len, const ParseOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>std::shared_ptr&lt; Base &gt;</type>
      <name>parse_file</name>
      <anchorfile>namespacemillijson.html</anchorfile>
      <anchor>a2d76d34b76c69908ba60999c35d1dcca</anchor>
      <arglist>(const char *path, const ParseOptions &amp;options)</arglist>
    </member>
    <member kind="function">
      <type>Type</type>
      <name>validate_file</name>
      <anchorfile>namespacemillijson.html</anchorfile>
      <anchor>a8e1ffeffc81b1d96d770451c18afc6d7</anchor>
      <arglist>(const char *path, const ParseOptions &amp;options)</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Yet another JSON parser</title>
    <filename>index.html</filename>
    <docanchor file="index.html">md__2github_2workspace_2README</docanchor>
  </compound>
</tagfile>
