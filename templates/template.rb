# frozen_string_literal: true

require "erb"
require "fileutils"
require "yaml"

module RBS
  class Template
    class Field
      attr_reader :name
      attr_reader :c_type

      def initialize(name:, c_type:)
        @name = name
        @c_type = c_type
      end

      def self.from_hash(hash)
        new(name: hash["name"], c_type: hash.fetch("c_type", "VALUE"))
      end

      # Returns a C expression that evaluates to the Ruby VALUE object for this field.
      def cached_ruby_value_expr
        case @c_type
        when "VALUE"
          @name
        when "rbs_node_t *", "rbs_node_list_t *"
          "#{@name}->cached_ruby_value"
        else
          "#{@name}->base.cached_ruby_value"
        end
      end
    end

    class Type
      attr_reader :full_name
      attr_reader :name
      attr_reader :c_function_name
      attr_reader :c_constant_name
      attr_reader :c_base_name
      attr_reader :c_struct_name
      attr_reader :c_type_enum_name
      attr_reader :parent_c_constant_name
      attr_reader :fields

      def initialize(yaml)
        @full_name = yaml["name"]
        @name = yaml["name"].split("::").last
        @c_function_name = if @full_name =~ /^RBS::Types/
          name = @full_name.gsub("::", "_").underscore
          name.gsub("_types_", "_") # TODO: should we rename these methods?
        else
          name = @full_name.gsub("::", "_").underscore
          name.gsub("_declarations_", "_decl_") # TODO: should we rename these methods?
        end
        @c_constant_name = @full_name.gsub("::", "_")
        @c_base_name = @c_constant_name.downcase
        @c_struct_name = "#{@c_base_name}_t"
        @c_type_enum_name = @c_base_name.upcase
        @fields = yaml["fields"]&.map { |field| Field.from_hash(field) }&.freeze || []
        @parent_c_constant_name = @full_name.split("::")[0..-2].join("::").gsub("::", "_")
        @expose_to_ruby = yaml.fetch("expose_to_ruby", true)
        @builds_ruby_object_internally = yaml.fetch("builds_ruby_object_internally", false)
      end

      # Every templated type will have a C struct created for it.
      # If this is true, then we will also create a Ruby class for it, otherwise we'll skip that.
      def expose_to_ruby?
        @expose_to_ruby
      end

      # When true, this object is expected to build its own Ruby VALUE object inside its `*_new()` function.
      # When false, the `*_new()` function will take a Ruby VALUE as its first argument.
      def builds_ruby_object_internally?
        @builds_ruby_object_internally
      end

      def constructor_parameters
        if builds_ruby_object_internally?
          @fields
        else
          [Field.new(name: "ruby_value", c_type: "VALUE")] + @fields
        end
      end
    end

    class << self
      def render(out_file)
        filepath = "templates/#{out_file}.erb"
        template = File.expand_path("../#{filepath}", __dir__)

        erb = read_template(template)
        extension = File.extname(filepath.gsub(".erb", ""))

        heading = <<~HEADING
          /*----------------------------------------------------------------------------*/
          /* This file is generated by the templates/template.rb script and should not  */
          /* be modified manually.                                                      */
          /* To change the template see                                                 */
          /* #{filepath + " " * (74 - filepath.size) } */
          /*----------------------------------------------------------------------------*/
        HEADING

        write_to = File.expand_path("../#{out_file}", __dir__)
        contents = heading + "\n" + erb.result_with_hash(locals)

        if (extension == ".c" || extension == ".h") && !contents.ascii_only?
          # Enforce that we only have ASCII characters here. This is necessary
          # for non-UTF-8 locales that only allow ASCII characters in C source
          # files.
          contents.each_line.with_index(1) do |line, line_number|
            raise "Non-ASCII character on line #{line_number} of #{write_to}" unless line.ascii_only?
          end
        end

        FileUtils.mkdir_p(File.dirname(write_to))
        File.write(write_to, contents)
      end

      private

      def read_template(filepath)
        template = File.read(filepath, encoding: Encoding::UTF_8)
        erb = erb(template)
        erb.filename = filepath
        erb
      end

      def erb(template)
        ERB.new(template, trim_mode: "-")
      end

      def locals
        config = YAML.load_file(File.expand_path("../config.yml", __dir__))
        {
          nodes: config.fetch("nodes").map { |node| Type.new(node) }.sort_by(&:full_name),
        }
      end
    end
  end
end

class String
  # From https://github.com/rails/rails/blob/a1f6a13f691e0929d40b7e1b1e0d31aa69778128/activesupport/lib/active_support/inflector/methods.rb#L99
  def underscore
    return self.dup unless /[A-Z-]|::/.match?(self)
    word = self.gsub("::", "/")
    word.gsub!(/(?<=[A-Z])(?=[A-Z][a-z])|(?<=[a-z\d])(?=[A-Z])/, "_")
    word.tr!("-", "_")
    word.downcase!
    word
  end
end

unless ARGV.size == 1
  $stderr.puts "Usage: ruby template.rb <out_file>"
  exit 1
end

out_file = ARGV.first
RBS::Template.render(out_file)
